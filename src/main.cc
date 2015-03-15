#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cctype>
#include "part_func.hh"

#define OPTION (15)
using namespace std;

void PrintHelpOption()
{
    cout << "\nParasoR (Parallel solution of RNA secondary structure prediction) 1.0.0\n"
         << "\twith Radiam method (RNA secondary structure analysis with deletion, insertion and substitution mutation).\n"
         << "This algorithm has been developed by xxxx xxxxxxxx.\n"
         << "We would like to express our gratitude to Vienna RNA package and CentroidFold for developing ParasoR.\n\n"
         << "You can achieve calculation of base pairing probability or accessibility for sequences having any length by ParasoR.\n\n";
    cout << "-------Option List------\n";
    cout << "-a\tuse accessibility \n\t(default: stem probability)\n"
         << "-p\tuse profiles\n"
         << "-c\tprint correlation coefficient of distance (eucledian, eSDC)\n"
         << "-r\tuse complementary sequence\n\n"
         << "-m [S or D or I or nan]\tcalculate mutated probability around single point Substitution or Deletion or Insertion from start to end\n"
         << "-s [num]\tstart point of base pairing probability (1~(sequence length-1)) \n"
         << "-e [num]\tend point of base pairing probability\n\t\tif you assign only a start position for one sequence, it'll be the sequence length.\n"
         << "-i [num]\tthe number of chunk id  0~(#chunk-1)\n\t\t(if you assign #chunk, it'll make connected file in the same way as connect option.)\n"
         << "-k [num]\tthe max number of chunk\n"
         << "-f [sequence]\tdirect input of sequence\n\n"
         << "-T [temperature]\tchange temperature\n"
         //<< "-d\tfor debug\n"
         << "-t\tkeep temporary douter files after connecting (only available with connect option)\n\n"
         << "--constraint [num]\tset the maximal span num\n"
         << "--input [filename]\tdeal filename as sequence file input\n"
         << "--outerinput [filename]\tdeal filename as douter file input\n"
         << "--name [name]\tprefix of db filename\n"
         << "--divide\tcalculate divided douters and make temporal douter files (default)\n"
         << "--connect\tconnect douters and make douter_inside & outside file\n"
         << "--stemdb\toutput stem probability database\n"
         << "--struct (or --struct=gamma)\toutput MEA structure (base pair having bpp (>= 1/(gamma+1)) \n"
         << "--image\toutput images of MEA (only with struct option)\n"
         << "--energy [.par file]\talso possible to use \"Turner2004\", \"Andronescu\", and \"Turner1999\" as an abbreviation\n"
         << "--bpp (or --bpp=minp)\tcalculate base pairing probability (output base pairing probability >= minp)\n"
         << "--stem\tcalculate stem probability\n"
         << "--text\ttext storage mode\n"
         << "--pre\tnot parallel computing\n"
         << "--help\tprint these sentences\n\n\n\n";
    cout << "-------Sample List------\n"
         << "Sample1:\n"
         << "\t\tParasoR --name seq1 -f [sequence] -i 0 -k 2\n"
         << "\t\tParasoR --name seq1 -f [sequence] -i 1 -k 2\n"
         << "\t\tParasoR --name seq1 -f [sequence] -i 2 -k 2 // Connecting;\n"
         << "\t\tParasoR --name seq1 -f [sequence] -s 1 --stem // Calculating stem probability;\n"
         << "\t\tParasoR --name seq1 -f [sequence] -i 0 -k 2 --stemdb // Making stemdb;\n"
         << "Sample2:\n"
         << "\t\tParasoR --name seq1 --energy A --input [input filename] --bpp -s 10 -e 20\n"
         << "\t\tParasoR --name seq1 --energy T --input [input filename] -i 0 -k 1 --stemdb -a\n"
         << "Sample3:\n"
         << "\t\tParasoR --name test -f [sequence] --pre --bpp\t\t// base pairing probability;\n"
         << "\t\tParasoR --name test -f [sequence] --pre --stem\t\t// stem probability;\n"
         << "\t\tParasoR --name test -f [sequence] --pre -a\t\t// accessibility;\n"
         << "\t\tParasoR --name test -f [sequence] --pre -p\t\t// profile;\n"
         << "\t\tParasoR --name test -f [sequence] --pre -a -p\t\t// profile string;\n"
         << "\t\tParasoR --name test -f [sequence] --pre --struct --image\t// MEA struct image;\n"
         << endl;
}

struct option* option()
{
    struct option *options = new struct option[OPTION+1];
    options[0].name = "constraint";
    options[1].name = "input";
    options[2].name = "outerinput";
    options[3].name = "name";
    options[4].name = "divide";
    options[5].name = "connect";
    options[6].name = "stemdb";
    options[7].name = "energy";
    options[8].name = "bpp";
    options[9].name = "text";
    options[10].name = "struct";
    options[11].name = "image";
    options[12].name = "stem";
    options[13].name = "pre";
    options[14].name = "help";
    for (int i = 0; i < OPTION; i++) {
        if (i == 4 || i == 5 || i == 6 || i == 9 || i == 11 || i == 12 || i == 13 || i == 14)
            options[i].has_arg = 0;
        else if (i == 10 || i == 8)
            options[i].has_arg = optional_argument;
        else
           options[i].has_arg = required_argument;
        options[i].flag = NULL; options[i].val = 1;
    }
    options[OPTION].name = 0; options[OPTION].has_arg = 0;
    options[OPTION].flag = 0; options[OPTION].val = 0;
    return options;
}

bool SetArg(int option_index, const char* optarg, Rfold::Arg& arg)
{
    if (option_index == 0 && optarg) {
        arg.constraint = atoi(optarg);
    } else if (option_index == 1 && optarg) {
        arg.input = string(optarg);
    } else if (option_index == 2 && optarg) {
        arg.outer_input = string(optarg);
        arg.init_calc = Rfold::Arg::Calc::Bpp;
    } else if (option_index == 3 && optarg) {
        arg.name = string(optarg);
    } else if (option_index == 4) {
        arg.init_calc = Rfold::Arg::Calc::Divide;
    } else if (option_index == 5) {
        arg.init_calc = Rfold::Arg::Calc::Connect;
    } else if (option_index == 6) {
        arg.init_calc = Rfold::Arg::Calc::Stemdb;
    } else if (option_index == 7) {
        arg.ene = string(optarg);
    } else if (option_index == 8) {
        arg.init_calc = Rfold::Arg::Calc::Bpp;
        if (optarg) arg.minp = atof(optarg);
    } else if (option_index == 9) {
        arg.text = true;
    } else if (option_index == 10) {
        if (optarg) arg.gamma = atof(optarg);
        arg.mea_flag = true;
        arg.init_calc = Rfold::Arg::Calc::Bpp;
    } else if (option_index == 11) {
        arg.image = true;
    } else if (option_index == 12) {
        arg.init_calc = Rfold::Arg::Calc::Bpp;
        arg.stem_flag = true;
    } else if (option_index == 13) {
        arg.pre_flag = true;
    } else if (option_index == 14) {
        PrintHelpOption();
        return false;
    }
    return true;
}

int GetMutType(const char* oprarg)
{
    string str = optarg;
    if (str.length() == 0) return Rfold::Arg::Mut::Sub;
    char c = toupper(str[0]);
    if (c == 'M' || c == 'S') return Rfold::Arg::Mut::Sub;
    else if (c == 'D') return Rfold::Arg::Mut::Del;
    else if (c == 'I') return Rfold::Arg::Mut::Ins;
    else return -1;
}


string GetSampleStr(int num)
{
    if (num == 0) {
        return string("atgttgtacctggaaaacaatgcccagtcccagtatagcgagccgcagtacacgaacctggggctcctgaacagcatggaccagcaggttcagaatggctcttcctccaccagcccctacaacacggagcacgcgcagaacagcgtcacggccccctcgccttacgcccagcccagctccacttttgatgccctctcgccctccccagccatcccttccaacacagactacccgggacctcacagcttcgacgtatcatttcaacaatccagcacagcaaagtctgcaacgtggacgtattccactgaactgaagaagctgtactgccagattgccaagacatgccccattcagatcaaagtgatgaccccaccaccccagggagctgtcatccgggctatgccagtctacaaaaaagcagggcacgtcaccgaagtggtcaaacgctgcccgaaccacgagctgagccgggagttcaatgaggggcagattgcacctcctagccacctgatcagagtggaaggaaacagccatgcccagtatgtggaagaccccatcactgggagacagagcgtgctggtcccatatgagccaccccaggttggtaccgagttcacaacagtcctgtacaacttcatgtgtaacagcagctgtgtaggagggatgaaccgtcgcccaattctcatcattgttacactggaaaccagagatgggcaagtcttgggccgccgatgttttgaagctcgcatttgcgcttgcccaggcagagatcgcaaagcagatgaggacagcatccgcaagcagcaagtctctgacagcacaaagaatggtgatgcttttcggcaaggaactcatggcatacagatgacatctatcaagaaaagacgttctccagatgatgagctcttgtacttgccggtgaggggacgagaaacatatgaaatgctactgaagatcaaagagtccctggaacctatgcagtaccttccccagcacacaattgagacttaccggcagcagcagcaacagcagcaccagcacttgctccagaagcagacctccattcagtcacagtcatcctatggctccaactcaccgccgctcagcaagatgaacagcatgaacaagctgccctcggtcagccagctcataaacccccagcagcgcaacgcactgaccccaaccaccatccctgacggcatgggaacaaacattcccatgatgggcactcacatggccatgaccggcgacatgaatgtcctcagccccacgcaggcgctgcctcctcccctctccatgccttcaacgtcccactgcactcctcctcctccataccccacagactgcagcattgtcagcttcttagcgaggttgggctgctcatcctgtgtggattatttcacgacccaagggctgaccaccatctatcatattgagcattactccatggatgatctggtgagcctcaagatcccggagcagttccgccacgccatctggaagggcatcctggaccaccggcagctccatgacttctcctctcctccccacctcctgcgtacccccagcggtgcctccaccgtcagcgtgggctccagcgaaacccggggggagcgggtcatcgatgcagtccgcttcactctccgccagaccatttccttcccgccccgcgacgagtggaacgatttcaacttcgacatggatgcccgccgcaacaaacagcaacgcatcaaggaggaaggggagtga");
        // sequence of p53;
    } else
        return string("UCAAACGCACCCUGCUCUCUACCGUUCUGAUUACACUUAUGUUUCCUCAGCCCUAGAGUCAUAGCCAGAGGUACCUUCUUAAUGAGAUAAGGUAGUAGACAUUGUGGCCUAUUACAUCGCAGCAUCUAAAAACCAUUAGCUCGGUAAGACUCAGUCCGACUAGGUAUGGAUGCCACCUCCCGUGUGGCGUAACCUAGUUACUUAUGCCCUUGUGGGUCAGCUAGUGCCGCAUUGGUAAACAAAUCUGCACGAUGCUAGACAGGACUGGCAUAGACAAACAUUAGACAAGGCCGACUCUCUCAUUGGCAAAACCAGGGUGUUCACGGGCAAACGUGGUGGGCGGGAGUCAUAGUUGGUGGCGCUUCCUAUGGCUAGGAGGGCUUGGCAGGAGGUCUCGCUCCUGUUUGUUCGACAAGAGGUAAAGGACUCUAGUGGAACGUGAAUCCAAUGGCUGCUUAGUACCCGGUCGGUUAUAGUAAAUGUCAGUUCCCCGGAGGAACGCGUUAACCCAACAGUUCCAAGUGGGGUCACUGAUGAGCCAUUCUGAAACCUUCAGCGCAUCGCUAGAGCGUAUUGGAUAACAUGUAGCAUCCACCUCACGUGACUUGAGUUGUCUGAAGGAGAUAUCGGAGUUGCACAGAGAGCCACUCUAUUUUGUGCGUUUGCAAAAAGACGCCCCCCUGGAACUGAUGACUUCGCAGCCUAACUGGGCCCUUUGAUGCGUGUGAAUGUACCGCUAGAAGGUACAACGGGUGCGACCAUUAUUAUUUAUCACCCUGUGGCCUUAUCUUGGUACAAACCAAUCGGGCAGCCUGCCCAUACGAGGCUUAAGCGUUCCGUUGUAUGAGAAGGCCACUGGACGUUCCUCGAUCAGACCUGUGCACCCGCGGUCGUGUCGGCCUAACCUUUAAUUCCGGCAUCUUAGUUUCAGGUUUUAGAAACGCUGUGCGAUCCAAAUGUUGCUAAUGCUCGUCUGGCUGCUGAUUAUUUCCCAAGCGGG");
        // random sequence;
}

char ComplementRNA(char c)
{
    if (c == '$') return c;
    c = toupper(c);
    switch(c) {
        case 'T': case 'U': return 'A';
        case 'G': return 'C';
        case 'C': return 'G';
        case 'A': return 'U';
        case 'N': return 'N';
        default:
            return 'N';
        /*
        case 'W': case 'K': case 'Y': case 'H': case 'B': case 'D': case 'N': return 'A';
        case 'R': case 'M': case 'V': return 'U';
        case 'S': return 'C';
        */
    }
}

char CapitalRNA(char c)
{
    if (c == '$') return c;
    c = toupper(c);
    switch(c) {
        case 'A': case 'C': case 'G': case'U': case 'N': return c;
        case 'T': case 'K': case 'Y': case 'B': return 'U';
        case 'W': case 'R': case 'M':
        case 'H': case 'V': case 'D': return 'A';
        case 'S': return 'C';
        default:
            std::cerr << "ambiguous character " << c << endl;
            return 'N';
    }
}

void GetCapitalRNA(string& seq)
{
    string str;
    transform(seq.begin(), seq.end(), back_inserter(str), CapitalRNA);
    seq = str;
}

void GetCompCapitalRNA(string& seq)
{
    string str;
    transform(seq.begin(), seq.end(), back_inserter(str), ComplementRNA);
    reverse(str.begin(), str.end());
    seq = str;
}

void RNATransform(Rfold::Arg& arg, string& seq)
{
    GetCapitalRNA(seq);
    if (arg.compl_flag) {
        arg.name += "_rev_";
        GetCompCapitalRNA(seq);
    }
}

void RNATransform(Rfold::Arg& arg) {
    RNATransform(arg, arg.str);
}

void NameTransform(string& name) {
    string rep = "";
    if (name.length() == 0) return;
    for (string::size_type pos = name.find("/"); pos != string::npos; pos = name.find("/", rep.length()+pos))
        name.replace(pos, 1, rep);
    for (string::size_type pos = name.find("\\"); pos != string::npos; pos = name.find("\\", rep.length()+pos))
        name.replace(pos, 1, rep);
    cout << "--file prefix: " << name << endl;
}

bool InitCondition(Rfold::Arg& arg)
{
    RNATransform(arg);
    NameTransform(arg.name);
    if (arg.id >= 0 && arg.init_calc == Rfold::Arg::Calc::Pre)
        arg.init_calc = Rfold::Arg::Calc::Divide;
    if (arg.id == arg.chunk) {
        if (arg.init_calc == Rfold::Arg::Calc::Divide)
            arg.init_calc = Rfold::Arg::Calc::Connect;
    } else if (arg.id > arg.chunk) {
        cerr << "id number is too large!" << endl;
        return false;
    } else if (arg.id < 0) {
        if (arg.pre_flag || arg.init_calc != Rfold::Arg::Calc::Bpp)
            arg.init_calc = Rfold::Arg::Calc::Pre;
    }
    return true;
}

void CalcOneSeq(Rfold::Arg& arg)
{
    if ((int)arg.str.length() == 0) return;
    if (!InitCondition(arg)) return;
    switch(arg.init_calc) {
        case Rfold::Arg::Calc::Divide:
            if (arg.id >= 0) {
                cout << "# Dividing (id: " << arg.id  << "/" << arg.chunk << ")" << endl;
                Rfold::ParasoR::DivideChunk(arg, false);
            }
            break;
        case Rfold::Arg::Calc::Pre:
            cout << "# Not parallel, Rfold computing" << endl;
            Rfold::ParasoR::PreviousCalculation(arg);
            break;
        case Rfold::Arg::Calc::Connect:
            cout << "# Connecting" << endl;
            Rfold::ParasoR::Connect(arg);
            break;
        case Rfold::Arg::Calc::Bpp:
            cout << "# Parallel bpp computing" << endl;
            Rfold::ParasoR::main(arg);
            break;
        case Rfold::Arg::Calc::Stemdb:
            if (arg.id == arg.chunk) {
                cout << "# Connecting stemdb" << endl;
            } else {
                cout << "# Making stemdb (id: " << arg.id << "/" << arg.chunk << ")" << endl;
            }
            Rfold::ParasoR::Stemdb(arg);
            break;
    }
}

void CalcStrucFasta(Rfold::Arg& arg)
{
    string str;
    string tname = arg.name;
    ifstream ifs(arg.input.c_str());
    cout << "-File Reading " << arg.input << "..." << endl;
    while (getline(ifs, str)) {
        if (str.length() == 0) continue;
        if (str[0] == '>') {
            CalcOneSeq(arg);
            arg.name = tname+str.substr(1);
            arg.str = "";
        } else
            arg.str += str;
    }
    if (arg.str != "")
        CalcOneSeq(arg);
}

void CheckCondition(Rfold::Arg& arg)
{
    if (arg.input != "") {
        CalcStrucFasta(arg);
    } else if (arg.str.length() > 0) {
        if (arg.start == 0 && arg.end < 0) arg.end = (int)arg.str.length();
        CalcOneSeq(arg);
    } else {
        arg.str = GetSampleStr(0);
        if (arg.start >= 0 && arg.end < 0) arg.end = (int)arg.str.length();
        arg.name = "Sample0_";
        CalcOneSeq(arg);
    }
}

int main(int argc, char** argv)
{
    int option_index;
    Rfold::Arg arg;
    try {
        struct option *options = option();
        while (1) {
            int opt = getopt_long(argc, argv, "m:w:s:e:bapi:k:f:cdrtT:", options, &option_index);
            if (opt == -1) break;
            switch(opt) {
                case 0: case 1:
                    if (!SetArg(option_index, optarg, arg)) {
                        delete[] options;
                        return 0;
                    }
                    break;
                case 's': if (optarg != NULL) arg.start = atoi(optarg)-1; break;
                case 'e': if (optarg != NULL) arg.end = atoi(optarg); break;
                case 'b': arg.init_calc = false; break;
                case 'i': if (optarg != NULL) arg.id = atoi(optarg); break;
                case 'k': if (optarg != NULL) arg.chunk = atoi(optarg); break;
                case 'a': arg.acc_flag = true; break;
                case 'p': arg.prof_flag = true; break;
                case 'c': arg.eSDC_flag = true; break;
                case 'r': arg.compl_flag = true; break;
                case 'd': arg.debug = true; break;
                case 'f': if (optarg != NULL) arg.str = string(optarg); break;
                case 't': arg.keep_flag = true; break;
                case 'T': if (optarg != NULL) arg.temp = atoi(optarg); break;
                case 'w': if (optarg != NULL) arg.window = atoi(optarg); break;
                case 'm': arg.mtype = (optarg != NULL) ? GetMutType(optarg) : Rfold::Arg::Mut::Sub; break;
                default: break;
            }
        }
        delete[] options;
        Rfold::Parameter::SetTemperature(arg.temp);
        Rfold::Parameter::ChangeEnergyParam(arg.ene);
        if (Rfold::Parameter::initialized)
            CheckCondition(arg);
    } catch (const char* str) {
        cout << "Catch error*" << endl;
        cout << str << endl;
    }
	return 0;
}