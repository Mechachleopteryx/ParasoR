#include "part_func.hh"

namespace Rfold {

DOUBLE ParasoR::GetInStem(LEN i, LEN j)
{
    int bp1 = seq.slidebp(i+1, j);
    DOUBLE temp = -INF;
    if (IsPair(bp1)) {
        if (debug) cout << seq.strget(i+1) << "-" << seq.strget(j) << endl;
        if (!Is_INF(Stem(alpha, i+1, j-1))) {
            temp = Logsumexp(temp, Logsum(Stem(alpha, i+1, j-1), LogLoopEnergy(i+1, j, i+2, j-1, seq)));
        }
        temp = Logsumexp(temp, Stemend(alpha, i+1, j-1));
    }
    return temp;
}

DOUBLE ParasoR::GetInMultiBif(LEN i, LEN j)
{
    DOUBLE temp = -INF;
    for (LEN k = i+TURN+1; k < j-TURN; k++)
        temp = Logsumexp(temp, Logsum(Multi1(alpha, i, k), Multi2(alpha, k, j)));
    return temp;
}

DOUBLE ParasoR::GetInMulti2(LEN i, LEN j)
{
    DOUBLE temp = Logsum(Multi2(alpha, i, j-1), logML_BASE);
    int type = seq.slidebp(i+1, j);
    if (IsPair(type)) {
        temp = Logsumexp(temp, Logsum(Stem(alpha, i, j), SumExtML(type, i, j+1, false, seq), logMLintern));
    }
    return temp;
}

DOUBLE ParasoR::GetInMulti1(LEN i, LEN j) {
    DOUBLE temp = Logsumexp(Multi2(alpha, i, j), Multibif(alpha, i, j));
    return temp;
}

DOUBLE ParasoR::GetInMulti(LEN i, LEN j) {
    DOUBLE temp = Logsumexp(Multibif(alpha, i, j),
                            Logsum(Multi(alpha, i+1, j), logML_BASE));
    return temp;
}

DOUBLE ParasoR::GetInStemend(LEN i, LEN j)
{
    LEN p = i, q = j+1;
    DOUBLE temp = -INF;
    if (q > p+_constraint || !IsPair(seq.slidebp(p, q))) return temp;
    else if (p > 0 && q <= seq.length) {
        temp = LogHairpinEnergy(p, q, seq);
        for (LEN ip = i; ip < j-TURN-1; ip++) {
            for (LEN jp = max(j-MAXLOOP+(ip-i), ip+TURN+2); jp <= j && (j-jp)+(ip-i) > 0; jp++) {
                if (IsPair(seq.slidebp(ip+1, jp))) {
                    temp = Logsumexp(temp, Logsum(Stem(alpha, ip, jp), LogLoopEnergy(p, q, ip+1, jp, seq)));
                }
            }
        }
        temp = Logsumexp(temp, Logsum(Multi(alpha, i, j), SumExtML(seq.sliderbp(p, q), j, i+1, false, seq),
                                      logMLclosing+logMLintern));
    }
    return temp;
}

void ParasoR::SetInsideMat(LEN i, LEN j)
{
    Stem(alpha, i, j) = GetInStem(i, j);
    Multibif(alpha, i, j) = GetInMultiBif(i, j);
    Multi2(alpha, i, j) = GetInMulti2(i, j);
    Multi1(alpha, i, j) = GetInMulti1(i, j);
    Multi(alpha, i, j) = GetInMulti(i, j);
    Stemend(alpha, i, j) = GetInStemend(i, j);
}

/* ///////////////////////////////////////////// */

DOUBLE ParasoR::GetOutStemend(LEN i, LEN j) {
    return (IsOnlyRange(i-1, j+1) ? Stem(beta, i-1, j+1) : -INF);
}

DOUBLE ParasoR::GetOutMulti(LEN i, LEN j)
{
    DOUBLE temp = -INF;
    if (IsOnlyRange(i-1, j))
        temp = Logsum(Multi(beta, i-1, j), logML_BASE);
    if (IsOnlyRange(i, j+1))
        temp = Logsumexp(temp, Logsum(Stemend(beta, i, j),
                                      SumExtML(seq.sliderbp(i, j+1), j, i+1, false, seq),
                                      logMLclosing+logMLintern));
    return temp;
}

DOUBLE ParasoR::GetOutMulti1(LEN i, LEN j)
{
    DOUBLE temp = -INF;
    for (LEN k = j+TURN+2; k < min(seq.length, i+_constraint+2); k++)
        temp = Logsumexp(temp, Logsum(Multibif(beta, i, k), Multi2(alpha, j, k)));
    return temp;
}

DOUBLE ParasoR::GetOutMulti2(LEN i, LEN j)
{
    DOUBLE temp = Multi1(beta, i, j);
    if (IsOnlyRange(i, j+1))
        temp = Logsumexp(temp, Logsum(Multi2(beta, i, j+1), logML_BASE));
    for (LEN k = max((LEN)0, j-_constraint-1); k < i-TURN-1; k++)
        temp = Logsumexp(temp, Logsum(Multibif(beta, k, j), Multi1(alpha, k, i)));
    return temp;
}

DOUBLE ParasoR::GetOutMultiBif(LEN i, LEN j) {
    return Logsumexp(Multi1(beta, i, j), Multi(beta, i, j));
}

DOUBLE ParasoR::GetOutStem(LEN i, LEN j)
{
    return GetOutStem(i, j, Logsum(Outer(alpha, i), Outer(beta, j)));
    // LEN p = i+1, q = j, type = seq.slidebp(p, q);
    // DOUBLE temp = -INF;
    // if (IsPair(type)) {
    //     temp = Logsum(Outer(alpha, i), Outer(beta, j), SumExtML(type, i, j+1, true, seq));
    //     if (q-p-1 >= TURN) {
    //         for (LEN ip = i; ip >= LeftRange(j); ip--) {
    //             for (LEN jp = min(ip+_constraint-1, min(j+MAXLOOP-(i-ip), seq.length-1)); jp >= j && (i-ip)+(jp-j) > 0; jp--) {
    //                 if (IsPair(seq.slidebp(ip, jp+1))) {
    //                     temp = Logsumexp(temp, Logsum(Stemend(beta, ip, jp), LogLoopEnergy(ip, jp+1, p, q, seq)));
    //                 }
    //             }
    //         }
    //     }
    //     temp = Logsumexp(temp, Logsum(Multi2(beta, i, j), SumExtML(type, i, j+1, false, seq), logMLintern));
    //     if (IsOnlyRange(i-1, j+1)) {
    //         temp = Logsumexp(temp, Logsum(Stem(beta, i-1, j+1), LogLoopEnergy(i, j+1, p, q, seq)));
    //     }
    // }
    // return temp;
}

void ParasoR::SetOutsideMat(LEN i, LEN j)
{
    if (i > 0 && j < seq.length) {
        Stemend(beta, i, j) = GetOutStemend(i, j);
        Multi(beta, i, j) = GetOutMulti(i, j);
        Multi1(beta, i, j) = GetOutMulti1(i, j);
        Multi2(beta, i, j) = GetOutMulti2(i, j);
        Multibif(beta, i, j) = GetOutMultiBif(i, j);
    }
    Stem(beta, i, j) = GetOutStem(i, j);
}

/* ///////////////////////////////////////////// */

DOUBLE ParasoR::GetStemDelta(LEN j, LEN i, bool inside)
{
    if (inside) {
        if (j-i == 1) return 0;     // extension of outer;
        else return Logsum(Stem(alpha, i, j), SumExtML(seq.slidebp(i+1, j), i, j+1, true, seq));
    } else {
        if (i-j == 1) return 0;     // extension of outer;
        else return Logsum(Stem(alpha, j, i), SumExtML(seq.slidebp(j+1, i), j, i+1, true, seq));
    }
}

DOUBLE ParasoR::GetStemDelta(LEN j, LEN i, int h, bool inside)
{
    if (inside) {
        if (i >= _start+1 || i == _start-h) {
            if (j-i == 1) return 0;     // extension of outer;
            else return Logsum(Stem(alpha, i, j), SumExtML(seq.slidebp(i+1, j), i, j+1, true, seq));
        }
    } else {
        if (i <= _end-1 || i == _end+h) {
            if (i-j == 1) return 0;     // extension of outer;
            else return Logsum(Stem(alpha, j, i), SumExtML(seq.slidebp(j+1, i), j, i+1, true, seq));
        }
    }
    return -INF;
}


DOUBLE ParasoR::CalcDalpha(LEN j, LEN i, int h, DOUBLE dalpha)
{
    DOUBLE value = GetStemDelta(j, i, h, true);
    if (Is_INF(value)) return -INF;
    if (ddebug) {
        cout << value << " " << Stem(alpha, i, j) << " " << SumExtML(seq.slidebp(i+1, j), i, j+1, true, seq)
        << " " << GetDo(alpha, i, h) << " " << dalpha << endl;
        if (j-i != 1) cout << seq.strget(i+1) << " " << seq.strget(j) << endl;
    }
    return Logsum(value, GetDo(alpha, i, h), -dalpha);
}

DOUBLE ParasoR::CalcDbeta(LEN j, LEN i, int h, DOUBLE dbeta)
{
    DOUBLE value = GetStemDelta(j, i, h, false);
    if (Is_INF(value)) return -INF;
    if (ddebug) {
        cout << value << " " << Stem(alpha, j, i) << " " << SumExtML(seq.slidebp(j+1, i), j, i+1, true, seq)
        << " " << GetDo(beta, i, h) << " " << dbeta << endl;
        if (i-j != 1) cout << seq.strget(j+1) << " " << seq.strget(i) << endl;
    }
    return Logsum(Logsum(value, GetDo(beta, i, h)), -dbeta);
}


void ParasoR::CalcInDeltaOuter(LEN j)
{
    cout.precision(15);
    if (ddebug) cout << j << endl;
    for (int h = 0; h <= _constraint; h++) {
        if (ddebug) cout << "------------" << h << endl;
        DOUBLE dalpha = 0, temp = -INF;
        for (int k = 1; k <= _constraint+1 && j-k >= 0; k++) {
            dalpha = Logsum(dalpha, GetDo(alpha, j-k, 0));
            if (j-k == _start-h || j >= _start+1) {
                DOUBLE tdalpha = CalcDalpha(j, j-k, h, dalpha);
                temp = Logsumexp(temp, tdalpha);
                if (ddebug) {
                    if (j-k == _start-h)
                        cout << "start-h " << tdalpha << " " << j-k << "=" << _start << "-" << h << endl;
                    cout << "value " << tdalpha << " " << GetDo(alpha, j-k, h) << " " << Stem(alpha, j-k, j) << endl;
                }
            }
            if (ddebug && !Is_INF(temp)) cout << k << ": " << temp << " " << dalpha << endl;
        }
        DOuter(alpha, j, h) = temp;
    }
    if (ddebug) cout << endl;
}

void ParasoR::CalcChunkInside(bool outer)
{
    if (ddebug) cout << "inside" << endl;
    for (LEN j = max(_start-_constraint, (LEN)1); j <= _end; j++) {
        if (ddebug) cout << j << endl;
        CalcInside(j);
        if (outer && j >= _start+1) CalcInDeltaOuter(j);
    }
    if (ddebug) PrintMat(true);
}

void ParasoR::CalcOutDeltaOuter(LEN j)
{
     for (int h = 0; h < _constraint+1; h++) {
        if (ddebug) cout << "------------" << h << endl;
        DOUBLE dbeta = 0;
        DOUBLE temp = -INF;
        for (int k = 1; j+k <= RightRange(j); k++) {
            dbeta = Logsum(dbeta, GetDo(beta, j+k, 0));
            if (j+k == _end+h || j+k <= _end-1) {
                DOUBLE tdbeta = CalcDbeta(j, j+k, h, dbeta);
                temp = Logsumexp(temp, tdbeta);
                if (ddebug) {
                    if (j+k == _end+h)
                        cout << "end+h+1 " << tdbeta << " " << j+k << "=" << _end << "+" << h << "+" << endl;
                    cout << "value " << tdbeta << " " << GetDo(alpha, j+k, h) << " " << Stem(alpha, j, j+k) << endl;
                }
            }
            if (ddebug && !Is_INF(temp)) cout << k << ": " << temp << " " << dbeta << endl;
        }
        DOuter(beta, j, h) = temp;
    }
    if (ddebug) cout << endl;
}

void ParasoR::CalcChunkOutside()
{
    if (ddebug) cout << "outside" << endl;
    for (LEN i = RightRange(_end); i >= _start; i--) {
        if (ddebug) cout << "i: " << i << endl;
        CalcInsideFromRight(i);
        if (i <= _end-1) CalcOutDeltaOuter(i);
    }
    if (ddebug) PrintMat(true);
}

void ParasoR::CalcDeltaInOut()
{
    CalcChunkInside();
    StoreDouterTemp(true);
    CalcChunkOutside();
    StoreDouterTemp(false);
}

/* ///////////////////////////////////////////// */

void ParasoR::ReadVec(Vec& tdouter, string& str)
{
    istringstream iss(str);
    vector<string> words;
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(words));
    for (vector<string>::iterator it = words.begin(); it != words.end(); it++) {
        tdouter.push_back(atof(it->c_str()));
    }
}

bool ParasoR::ReadBinVec(int h, Vec& tdouter, ifstream& ifs)
{
    DOUBLE value;
    for (int i = 0; i < h; i++) {
        if (ifs.eof()) return false;
        ifs.read((char*)&value, sizeof(DOUBLE));
        tdouter.push_back(value);
    }
    return true;
}

bool ParasoR::ReadDouterInside(Mat& douter, string filename, Vec& first_douter)
{
    string str;
    ifstream ifs(filename.c_str());
    if (!ifs) return false;
    if (!noout) cout << "Reading " << filename << endl;
    for (LEN i = 0; getline(ifs, str); i++) {
        if (first_douter.size() > 0 && i == 0) {
            douter.push_back((first_douter));
        } else {
            douter.push_back(Vec());
            ReadVec(douter[i], str);
        }
    }
    if (!noout) cout << "--size: " << douter.size() << endl;
    return douter.size() > 0;
}

bool ParasoR::ReadBinDouterInside(Mat& douter, string filename, Vec& first_douter)
{
    int h = 0;
    if (!noout) cout << "Reading " << filename << endl;
    ifstream ifs(filename.c_str(), ios::binary);
    if (!ifs || (h = GetColumn(ifs)) <= 0) return false;
    if (!noout) cout << "column size: " << h << endl;
    for (LEN i = 0; ; i++) {
        Vec temp = Vec();
        if (ReadBinVec(h, temp, ifs)) {
            if (first_douter.size() > 0 && i == 0) {
                douter.push_back((first_douter));
            } else douter.push_back(temp);
        } else  break;
    }
    if (!noout) cout << "--size: " << douter.size() << endl;
    return douter.size() > 0;
}

bool ParasoR::ReadDouterOutside(Mat& douter, string filename, Vec& first_douter)
{
    string str;
    ifstream ifs(filename.c_str());
    if (!noout) cout << "Reading " << filename << endl;
    if (!ifs) return false;
    for (LEN i = 0; getline(ifs, str); i++) {
        douter.push_back(Vec());
        ReadVec(douter[i], str);
    }
    if (first_douter.size() > 0 && douter.size() > 0)
        douter[douter.size()-1] = first_douter;
    if (!noout) cout << "--size: " << douter.size() << endl;
    return douter.size() > 0;
}

bool ParasoR::ReadBinDouterOutside(Mat& douter, string filename, Vec& first_douter)
{
    int h = 0;
    if (!noout) cout << "Reading " << filename << endl;
    ifstream ifs(filename.c_str(), ios::binary);
    if (!ifs || (h = GetColumn(ifs)) <= 0) return false;
    if (!noout) cout << "column size: " << h << endl;
    for ( ; ; ) {
        Vec temp = Vec();
        if (ReadBinVec(h, temp, ifs)) {
            douter.push_back(temp);
        } else  break;
    }
    if (first_douter.size() > 0 && douter.size() > 0)
        douter[douter.size()-1] = first_douter;
    if (!noout) cout << "--size: " << douter.size() << endl;
    return douter.size() > 0;
}


void ParasoR::GetSumDouterList(const Vec& old_douter, Vec& sum_douter, bool inside)
{
    if (inside) {
        DOUBLE acc = 0.0;
        for (int i = 0; i <= _constraint && i < (int)old_douter.size(); i++) {
            sum_douter[i] = acc;
            acc = Logsum(acc, old_douter[(LEN)old_douter.size()-1-i]);
        }
    } else {
        DOUBLE acc = 0.0;
        for (int i = 0; i <= _constraint && i < (int)old_douter.size(); i++) {
            sum_douter[i] = acc;
            acc = Logsum(acc, old_douter[i]);
        }
    }
}

DOUBLE ParasoR::GetDenominator(const Vec& douter, const Vec& sum_douter, bool inside)
{
    if (inside) {
        DOUBLE value = -INF;
        for (int h = 0; h <= _constraint && h < (int)sum_douter.size(); h++) {
            if (!Is_INF(sum_douter[h]))
            value = Logsumexp(value, douter[h]-sum_douter[h]);
            if (ddebug) cout << "deno " << douter[h] << " " << sum_douter[h] << endl;
        }
        return value;
    } else {
        DOUBLE value = -INF;
        for (int h = 0; h <= _constraint && h < (int)sum_douter.size(); h++) {
            if (!Is_INF(sum_douter[h]))
            value = Logsumexp(value, douter[h]-sum_douter[h]);
            if (ddebug) cout << "deno " << douter[h] << " " << sum_douter[h] << endl;
        }
        return value;
    }
}

DOUBLE ParasoR::GetNumerator(const Mat& douter, const Vec& sum_douter, int k, bool inside)
{
    if (inside) {
        DOUBLE value = -INF;
        for (int h = 0; h <= _constraint && h < (int)sum_douter.size(); h++) {
            if (ddebug) cout << "*****" << douter[k][h] << " " << douter[k-1][0] << " " << sum_douter[h] << endl;
            value = Logsumexp(value, douter[k][h]+douter[k-1][0]-sum_douter[h]);
        }
        return value;
    } else {
        DOUBLE value = -INF;
        for (int h = 0; h <= _constraint && h < (int)sum_douter.size(); h++) {
            if (ddebug) cout << "*****" << douter[k][h] << " " << douter[k+1][0] << " " << sum_douter[h] << endl;
            value = Logsumexp(value, douter[k][h]+douter[k+1][0]-sum_douter[h]);
        }
        return value;
    }
}

void ParasoR::ConnectInDo(Vec& old_douter, Mat& douter, int tid)
{
    Vec new_douter = Vec(douter.size(), 0.0);
    Vec sum_douter = Vec(old_douter.size(), 0.0);
    if (tid != 0)
        GetSumDouterList(old_douter, sum_douter, true);
    for (int i = 1; i < new_douter.size(); i++) {
        if (ddebug) cout << "--------" << i << " " << seq.strget(seq.length/chunk*tid+i)<< endl;
        DOUBLE denominator = (i == 1) ? douter[i-1][0] : //start position;
                                      GetDenominator(douter[i-1], sum_douter, true);
        DOUBLE numerator = GetNumerator(douter, sum_douter, i, true);
        new_douter[i] = numerator-denominator;
       if (ddebug)
            cout << new_douter[i] << " " << numerator << " " << denominator << endl;
    }
    StoreDouter(GetDoFile(true), new_douter, true, tid > 0);
    old_douter = new_douter;
}

void ParasoR::ConnectOutDo(Vec& old_douter, Mat& douter, int tid)
{
    Vec new_douter = Vec(douter.size(), 0.0);
    Vec sum_douter = Vec(old_douter.size(), 0.0);
    if (tid != chunk-1)
        GetSumDouterList(old_douter, sum_douter, false);
    for (LEN i = douter.size()-2; i >= 0; i--) {
        if (ddebug) cout << "--------" << i << " " << seq.strget(seq.length/chunk*tid+i+1) << endl;
        DOUBLE denominator = (i == douter.size()-2) ? douter[i+1][0] : // start position;
                             GetDenominator(douter[i+1], sum_douter, false);
        DOUBLE numerator = GetNumerator(douter, sum_douter, i, false);
        new_douter[i] = numerator-denominator;
        if (ddebug)
            cout << new_douter[i] << " " << numerator << " " << denominator << endl;
    }
    StoreDouter(GetDoFile(false), new_douter, false, tid < chunk-1);
    old_douter = new_douter;
}

void ParasoR::ConnectDo(bool keep_flag)
{
    bool iflag = true, oflag = true;
    Init();
    Vec old_douter = Vec(1, 0);
    Vec first_douter;
    for (int i = 0; i < chunk; i++) {
        Mat douter;
        iflag = (binary) ? ReadBinDouterInside(douter, GetTempFileList(true, i), first_douter)
                : ReadDouterInside(douter, GetTempFileList(true, i), first_douter);
        if (iflag) {
            ConnectInDo(old_douter, douter, i);
            first_douter = douter[(LEN)douter.size()-1];
        } else break;
    }
    if (!noout) cout << "-Complete " << GetDoFile(true) << endl;
    if (!keep_flag && iflag) RemoveTemp(true);
    old_douter = Vec(1, 0);
    first_douter.clear();
    for (int i = chunk-1; i >= 0; i--) {
        Mat douter;
        oflag = (binary) ? ReadBinDouterOutside(douter, GetTempFileList(false, i), first_douter)
                : ReadDouterOutside(douter, GetTempFileList(false, i), first_douter);
        if (oflag) {
            ConnectOutDo(old_douter, douter, i);
            first_douter = douter[0];
        } else break;
    }
    if (!noout) cout << "-Complete " << GetDoFile(false) << endl;
    if (!keep_flag && oflag) RemoveTemp(false);
}

/* ///////////////////////////////////////////// */




DOUBLE ParasoR::bpp(LEN i, LEN j, bool deb)
{
    if (i > j) swap(i, j);
    if (abs(i-j) < TURN) return 0.0;
    DOUBLE stack = Logsum(Stem(alpha, i, j-1), Stem(beta, i-1, j), LogLoopEnergy(i, j, i+1, j-1, seq));
    DOUBLE stemend = Logsum(Stemend(alpha, i, j-1), Stem(beta, i-1, j));
    double temp = 0.0;
    if (!Is_INF(stack)) temp += exp(Logsum(stack, -Outer(alpha, seq.length)));
    if (!Is_INF(stemend)) temp += exp(Logsum(stemend, -Outer(alpha, seq.length)));
    if (deb) {
        cout << "stack " << Stem(alpha, i, j-1) << " " << Stem(beta, i-1, j) << " " << stack << endl;
        cout << "stemend " << Stemend(alpha, i, j-1) << " " << Stem(beta, i-1, j) << " " << stemend << endl;
        cout << "dpp " << i << " " << j << " " << temp << endl;
    }
    return temp;
 }

DOUBLE ParasoR::bppDelta(LEN i, LEN j, bool deb)
{
    if (i > j) swap(i, j);
    if (abs(i-j) < TURN) return 0.0;
    DOUBLE stack = Logsum(Stem(alpha, i, j-1), Stem(beta, i-1, j), LogLoopEnergy(i, j, i+1, j-1, seq));
    DOUBLE stemend = Logsum(Stemend(alpha, i, j-1), Stem(beta, i-1, j));
    if (deb && exp(Logsumexp(stack, stemend))-1.0 > 1e-6) {
        cout << "----error " << i << " " << j << endl;
        cout << "---- " << Stem(alpha, i, j-1) << " " << Stem(beta, i-1, j) << " " << LogLoopEnergy(i, j, i+1, j-1, seq) << endl;
        cout << "---- " << Stemend(alpha, i, j-1) << " " << Stem(beta, i-1, j) << endl;
        cout << "dpp " << exp(Logsumexp(stack, stemend)) << endl;
    }
    return exp(Logsumexp(stack, stemend));
}

void ParasoR::WriteBpp(Mat& data)
{
    data.clear();
    data = Mat(seq.length, Vec(_constraint, 0.0));
    for (LEN i = 1; i < seq.length; i++) {
        for (LEN j = i+1; j <= RightBpRange(i); j++)
            data[i-1][j-i-1] = bpp(i, j);
    }
}

void ParasoR::WriteAcc(Mat& data)
{
    data.clear();
    data = Mat(seq.length, Vec(_constraint+1, 0.0));
    for (LEN i = 1; i <= seq.length; i++) {
        for (LEN j = i; j <= min(seq.length, i+_constraint); j++) {
            data[i-1][j-i] = acc(i, j);
            if (data[i-1][j-i] > 1.0) {
                if (data[i-1][j-i]-1.0 >= 1.0e-11) {
                    ;//cerr << "error? " << i << " " << j << " " << data[i-1][j-i]-1.0 << endl;
                }
                data[i-1][j-i] = 1.0;
            }
        }
    }
}

void ParasoR::WriteStemProb(Vec& stem, const Mat& mat)
{
    stem.clear();
    stem = Vec((LEN)mat.size());
    for (LEN i = 0; i < (LEN)mat.size(); i++) {
        for (LEN j = 0; j < (LEN)mat[i].size(); j++) {
            stem[i] += mat[i][j];
            stem[i+j+1] += mat[i][j];
        }
    }
}

void ParasoR::WriteStemProb(Vec& stem)
{
    stem.clear();
    stem = Vec(seq.length, 0.0);
    for (LEN i = 1; i <= seq.length; i++) {
        for (LEN j = i+1; j <= i+_constraint && j <= seq.length; j++) {
            DOUBLE value = bpp(i, j);
            stem[i-1] += value;
            stem[j-1] += value;
        }
    }
}

void ParasoR::OutputStemProb(bool _acc)
{
    cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    for (LEN i = 1; i <= seq.length; i++) {
        DOUBLE P[3] = { 1.0, 0.0, 0.0 };
        for (LEN j = max((LEN)1, i-_constraint); j <= min(seq.length, i+_constraint); j++) {
            if (!_acc && j == i) continue;
            DOUBLE value = ((_acc) ? acc(i, j) : bpp(i, j));
            if (_acc) { cout << "* " << i << " " << j << " " << value << endl; continue; }
            P[0] -= value;
            (j < i) ? P[2] += value : P[1] += value;
        }
        if (!_acc) cout << "* " << seq.strget(i) << " " << i << " " << P[1] << " " << P[2] << endl;
    }
}

void ParasoR::OutputBppCond(bool _acc)
{
    cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    if (_acc) {
        for (LEN i = 1; i <= seq.length; i++) {
            DOUBLE value = acc(i, i+1);
            cout << "*\t" << i << "\t" << value << endl;
        }
    } else {
        for (LEN i = 1; i <= seq.length; i++) {
            for (LEN j = max((LEN)1, i-_constraint); j <= min(seq.length, i+_constraint); j++) {
                if (j == i) continue;
                DOUBLE value = bpp(i, j);
                cout << "*\t" << i << "\t" << j << "\t" << value << endl;
            }
        }
    }
}

string ParasoR::GetDoFile(bool inside)
{
    ostringstream os;
    if (inside) {
        os << TMP << name << "douter_inside_" << _constraint;
        if (!binary) os << ".txt";
    } else {
        os << TMP << name << "douter_outside_" << _constraint;
        if (!binary) os << ".txt";
    }
    return os.str();
}

string ParasoR::GetStemFile(bool acc, bool prof)
{
    ostringstream os;
    if (acc && prof) {
        os << STMP << name << "prof_" << _window << "_" << _constraint;
    } else if (acc) {
        os << STMP << name << "acc_" << _window << "_" << _constraint;
    } else {
        os << STMP << name << "stem_" << _constraint;
    }
    if (!binary) os << ".txt";
    return os.str();
}

string ParasoR::GetDividedStemFile(bool acc, bool prof)
{
    ostringstream os;
    if (acc && prof) {
        os << STMP << name << "profd_" << id << "_" << chunk << "_" << _window << "_" << _constraint;
    } else if (acc) {
        os << STMP << name << "accd_" << id << "_" << chunk << "_" << _window << "_" << _constraint;
    } else {
        os << STMP << name << "stemd_" << id << "_" << chunk << "_" << _constraint;
    }
    if (!binary) os << ".txt";
    return os.str();
}

string ParasoR::GetTempFileList(bool inside, int tid)
{
    if (tid < 0) tid = id;
    ostringstream os;
    if (inside) {
        os << TMP << name << "temp_inside_douter_" << tid << "_" << chunk << "_" << _constraint;
        if (!binary) os << ".txt";
    } else {
        os << TMP << name << "temp_outside_douter_" << tid << "_" << chunk << "_" << _constraint;
        if (!binary) os << ".txt";
    }
    return os.str();
}

void ParasoR::WriteDouterTemp(ofstream& ofs, Mat& mat)
{
    ostream_iterator<DOUBLE> oit(ofs, "\t");
    for (Mat::iterator it = mat.begin(); it != mat.end(); it++) {
        copy(it->begin(), it->begin()+(int)it->size(), oit);
        ofs << endl;
    }
}

void ParasoR::WriteBinDouterTemp(ofstream& ofs, Mat& mat)
{
    int h = mat[0].size();
    ofs.write((char*)&h, sizeof(int));
    for (LEN i = 0; i < (LEN)mat.size(); i++) {
        for (LEN j = 0; j < (LEN)(mat[i].size()); j++) {
            ofs.write((char*)&(mat[i][j]), sizeof(DOUBLE));
        }
    }
}

void ParasoR::StoreDouterTemp(bool inside)
{
    string file = GetTempFileList(inside);
    if (binary) {
        ofstream ofs(file.c_str(), ios::binary);
        WriteBinDouterTemp(ofs, (inside) ? alpha.douter : beta.douter);
        ofs.close();
        if (!noout) cout << "-Written (binary) " << file << endl;
    } else {
        ofstream ofs(file.c_str());
        ofs.precision(PREC);
        WriteDouterTemp(ofs, (inside) ? alpha.douter : beta.douter);
        if (!noout) cout << "-Written " << file << endl;
        ofs.close();
    }
}

void ParasoR::StoreDouter(string filename, Vec& douter, bool inside, bool app)
{
    if (douter.size() == 0) {
        cerr << "Cannot store douter (maybe have a problem in reading temp douter file)" << endl;
        abort();
    }
    int start = (inside) ? 1 : 0;
    int end = (inside) ? 0 : -1;
    if (binary) {
        int const_size = douter.size()+(end-start);
        ofstream ofs(filename.c_str(), ((app) ? ios::app : ios::trunc) | ios::binary);
        ofs.write((char*)&const_size, sizeof(int));
        for (int i = start; i < (int)douter.size()+end; i++)
            ofs.write((char*)(&douter[i]), sizeof(DOUBLE));
        ofs.close();
    } else {
        ofstream ofs(filename.c_str(), (app) ? ios::app : ios::trunc);
        ofs.precision(PREC);
        ostream_iterator<DOUBLE> oit(ofs, "\t");
        copy(douter.begin()+start, douter.begin()+(int)douter.size()+end, oit);
        ofs << endl;
        ofs.close();
    }
}

void ParasoR::StoreStem(string filename, Vec& vec, bool acc)
{
    assert(vec.size() > 0);
    if (acc) ;
    else if (*max_element(vec.begin(), vec.end())-1.0 >= 0.1 || *min_element(vec.begin(),vec.end()) <= -0.1)
        throw "Value overflow or underflow";
    if (binary) {
        int const_size = vec.size();
        ofstream ofs(filename.c_str(), ios::binary);
        ofs.write((char*)&const_size, sizeof(int));
        for (int i = 0; i < _end-_start && i < (int)vec.size(); i++) {
            if (!acc) vec[i] = min(max((DOUBLE)0, vec[i]), (DOUBLE)1);
            ofs.write((char*)(&vec[i]), sizeof(DOUBLE));
        }
        ofs.close();
    } else {
        ofstream ofs(filename.c_str(), ios::trunc);
        ofs.precision(PREC);
        ostream_iterator<DOUBLE> oit(ofs, "\t");
        copy(vec.begin(), vec.begin()+(int)vec.size(), oit);
        ofs.close();
    }
}

// void ParasoR::StoreProf(string filename, vector<char>& vec)
// {
//     assert(vec.size() > 0);
//     if (binary) {
//         int const_size = vec.size();
//         ofstream ofs(filename.c_str(), ios::binary);
//         ofs.write((char*)&const_size, sizeof(int));
//         for (int i = 0; i < _end-_start && i < (int)vec.size(); i++) {
//             ofs.write((char*)(&vec[i]), sizeof(char));
//         }
//         ofs.close();
//     } else {
//         ofstream ofs(filename.c_str(), ios::trunc);
//         ostream_iterator<char> oit(ofs, "\t");
//         copy(vec.begin(), vec.begin()+(int)vec.size(), oit);
//         ofs.close();
//     }
// }

void ParasoR::PrintMat(bool is_alpha)
{
    if (is_alpha) {
        cout << "--alpha" << endl;
        alpha.Print(seq.substr(0));
    } else {
        cout << "--beta" << endl;
        beta.Print(seq.substr(0));
    }
}

void ParasoR::Init(bool full)
{
    if (full) {
        alpha = Matrix(_end-_start+1, _constraint, true);
        beta = Matrix(_end-_start+1, _constraint, false);
    } else {
        if (cut) {
            seq.CutSequence(max(0, _start-CONST*_constraint), min(seq.length, _end+CONST*_constraint));
        }
        alpha = Matrix(CONST*_constraint, _constraint, true);
        beta = Matrix(CONST*_constraint, _constraint, false);
    }
    alpha.SetIndex(_start, _end);
    beta.SetIndex(_start, _end);
}

void ParasoR::InitBpp(bool full)
{
    if (!alpha.isSet() || !beta.isSet()) {
        if (full) {
            alpha = Matrix(_end-_start+1, _constraint, true);
            beta = Matrix(_end-_start+1, _constraint, false);
        } else {
            alpha = Matrix(CONST*_constraint, _constraint, true);
            beta = Matrix(CONST*_constraint, _constraint, false);
        }
    }
    LEN left = max(_start-_constraint-10, (LEN)0);
    LEN right = min(_end+_constraint+10, seq.length);
    if (full && cut) {
        seq.CutSequence(left, right);
    }
    SetIndex(left, right, false);
}

void ParasoR::SetSequence(const string& sequence, bool out)
{
    if (out) {
        cout << "# " << sequence.substr(0, 50);
        if (sequence.length() > 50) cout << "...";
        cout << endl;
    }
    string str = "$" + sequence;
    vector<char> num_seq;
    transform(str.begin(), str.end(), back_inserter(num_seq), Base());
    seq = Sequence(str, num_seq, (LEN)sequence.length());
}

void ParasoR::RemoveTemp(bool inside)
{
    cout << "RemoveTemp" << endl;
    string file;
    for (int i = 0; i < chunk; i++) {
        file = GetTempFileList(inside, i);
        remove(file.c_str());
    }
}

void ParasoR::RemoveStem(bool acc, bool prof)
{
    cout << "RemoveStem" << endl;
    for (int i = 0; i < chunk; i++) {
        ostringstream os;
        id = i;
        string file = GetDividedStemFile(acc, prof);
        remove(file.c_str());
    }
}

void ParasoR::ConcatDo()
{
    ostringstream os;
    if (binary) {
        os << " binary ";
#if defined (LONG)
        os << " long ";
#elif defined (SHORT)
        os << " short ";
#endif
    }
    os << "constraint=" << _constraint;
     if (binary) {
        Douter_concat::ConcatBin(GetDoFile(true));
        Douter_concat::ConcatBinReverse(GetDoFile(false));
        if (!noout) cout << "-Complete Do file." << endl;
    } else {
        Douter_concat::Concat(GetDoFile(true));
        Douter_concat::ConcatReverse(GetDoFile(false));
        if (!noout) cout << "-Complete Do file." << endl;
    }
}

void ParasoR::DivideChunk(Arg& arg, bool shrink)
{
    if (arg.str.length() == 0) return;
    ParasoR rfold;
    rfold.SetBasicParam(arg, shrink);
    if (rfold.seq.length/arg.chunk > (LEN)INT_MAX) {
        cout << INT_MAX << " " << rfold.seq.length << endl;
        cerr << "Too long sequence for a single chunk.\nPlease increase the chunk number more than  " << rfold.seq.length/(LEN)INT_MAX << endl;
    } else if (rfold.SetChunkId(arg.id, arg.chunk)) {
        if (rfold.seq.length/rfold.chunk) {
            rfold.cut = true;
            rfold.CalcDeltaInOut();
        } else if (rfold.id == 0 && rfold.chunk == 1) {
            Connect(rfold, shrink);
        }
    } else {
        cerr << "Too many chunk for this sequence len: " << rfold.seq.length << endl;
    }
}


void ParasoR::PreviousCalculation(Arg& arg, bool shrink)
{
    if (arg.str.length() == 0) return;
    ParasoR rfold;
    rfold.SetWindow(arg.window, arg.acc_flag & !arg.prof_flag);
    rfold.SetBasicParam(arg, shrink);
    rfold.SetRange(1, arg.str.length());
    if (arg.prof_flag) rfold.CalcAllAtOnce(Out::PROF);
    else if (arg.acc_flag) rfold.CalcAllAtOnce(Out::ACC);
    else if (arg.mea_flag) rfold.CalcAllAtOnce(Out::MEA, arg.gamma);
    else if (arg.stem_flag) rfold.CalcAllAtOnce(Out::STEM);
    else rfold.CalcAllAtOnce(Out::BPP, arg.minp);
}

void ParasoR::main(Arg& arg, bool shrink)
{
    if (arg.str.length() == 0) return;
    ParasoR rfold;
    rfold.SetWindow(arg.window, arg.acc_flag & !arg.prof_flag);
    if (arg.mtype == Arg::Mut::Ins && arg.constraint == arg.str.length())
        rfold.SetBasicParam(arg, false);
    else
        rfold.SetBasicParam(arg, shrink);
    if (arg.mtype < 0) {
        if (arg.end < 0) {
            arg.end = arg.str.length();
        }
        if (arg.end > arg.start) {
            rfold.SetBpRange(arg.start, arg.end);
            rfold.cut = true;
            if (arg.mea_flag) {
                rfold.CalcMEA(true, arg.image, arg.prof_flag);
            } else if (arg.prof_flag) {
                rfold.CalcProf(arg.acc_flag);
            } else if (arg.acc_flag) {
                rfold.SetWindow(max(2, arg.window));
                rfold.CalcAcc();
            } else if (arg.stem_flag) {
                rfold.CalcStem();
            } else
                rfold.CalcBpp(true, arg.minp);
        }
    } else {
        rfold.MutatedStem(arg);
    }

}


}