#include <iostream>
#include <getopt.h>
#include <cstdlib>

#include <sys/stat.h>
#include "part_func.hh"

#define OPTION (31)
#define MINDIRLEN (8)

using namespace std;

void PrintHelpOption()
{
    cout << "\nParasoR (Parallel solution of RNA secondary structure prediction) 1.1.0\n"
         << "\twith Radiam method (RNA secondary structure analysis with deletion, insertion and substitution mutation).\n"
         << "This algorithm has been developed in github.com/carushi/ParasoR.\n"
         << "We would like to express our gratitude to Vienna RNA package and CentroidFold for developing ParasoR.\n\n"
         << "You can achieve calculation of base pairing probability or accessibility for sequences of any nucleotides long by ParasoR.\n\n";
    cout << "-------Option List------\n\n"
         << "--pre\tnot-parallel computing (Rfold and CapR)\n"
         << "(without --pre)\tparallel computing (ParasoR) \n"
         << "--help\tprint detailed help\n\n";
    cout << "-------Sample List------\n"
         << "Rfold and CapR:\n"
         << "\t\t./ParasoR --pre --bpp // use a default p53 sequence;\n"
         << "\t\t./ParasoR --pre --stem --input [input fasta]\n"
         << "\t\t./ParasoR --pre --prof --struct -f [sequence]\n"
         << "\nParallel computing:\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] -i 0 -k 2 // Divide procedure #0\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] -i 1 -k 2 // Divide procedure #1\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] -i 2 -k 2 // Connect procedure;\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] --stem // Calculate stem probability for an entire sequence;\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] -i 0 -k 2 --stemdb // Construct stemdb for the first fragment;\n"
         << "\t\t./ParasoR --name seq1 -f [sequence] -i 0 -k 2 --stemdb --stdout // Print stemdb;\n"
         << "\nSample2:\n"
         << "\t\t./ParasoR --name seq1 --energy A --input [input fasta] --bpp -s 10 -e 20\n"
         << "\t\t./ParasoR --name seq1 --energy ../energy_param/rna_turner2004_new.par --input [input fasta] -i 0 -k 1 --stemdb -a\n"
         << "\nSample3: (not parallel)\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --bpp\t\t// base pairing probability;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --stem\t// stem probability;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --acc\t\t// accessibility;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --prof\t\t// profile;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --motif\t\t// profile string;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --struct --image\t// centroid struct image;\n"
         << "\t\t./ParasoR --name test -f [sequence] --pre --struct --mfe\t// mfe structure;\n"
         << endl;
}

void PrintDetailedHelpOption()
{
    cout << "ParasoR (Parallel solution of RNA secondary structure prediction)\n\n"
         << "Detailed options:\n\n"
         << "-------Running mode------\n\n"
         << "--pre\n\tnot-parallel computing (Rfold and CapR)\n"
         << "(without --pre)\n\tparallel computing (ParasoR) \n"
         << "--help\n\tprint detailed help\n"
         << "--divide\n\tcalculate divided douters and make temporal douter files (default)\n"
         << "\t-i [num]\n\t\tthe number of chunk id  0~(#chunk-1)\n\t\t(if you assign #chunk, it'll make connected file in the same way as connect option.)\n"
         << "\t-k [num]\n\t\tthe max number of chunk\n"
         << "--connect\n\tconnect douters and make douter_inside & outside file\n"
         << "--stemdb\n\toutput stem probability database\n"
         << "-m [S or D or I or nan]\n\tcalculate mutated probability around single point Substitution or Deletion or Insertion from start to end\n"
         << "\t--mout [outfile or nan]\n\t\toutput a result of mutatiing simulation to stdout or outfile\n"
         << "--save_memory\n\tsave memory mode in Divide and Connect procedure\n"
         << "--text\n\ttext storage mode with low accuracy\n\n";
    cout << "-----Subject features-----\n\n"
         << "--stem\n\tcalculate stem probability\n"
         << "--bpp (or --bpp=minp)\n\tcalculate base pairing probability (output base pairing probability >= minp)\n"
         << "--entropy\n\tcalculate entropy (sum of -plog2p)\n";
    cout << "--acc\t(-a)\n\tuse accessibility\n\t(default: stem probability)\n"
         << "--prof\t(-p -a)\n\tuse profile vectors\n"
         << "--motif\t(-p)\n\tprint a motif sequence\n"
         << "--struct (or --struct=gamma)\n\toutput a centroid structure with gamma centroid estimator (whose base pairs having bpp (>= 1/(gamma+1) and length is less than 600 nt) \n"
         << "--mfe\n\tcompute minimum free energy (MFE) structure (only valid with pre and struct or bpp option)\n"
         << "--image\n\toutput images of gamma centroid estimated structure (only with struct option)\n"
         << "--boundary\n\tcalculate a probability having no base pair with its right side at each position (only valid without pre option)\n"
         << "-c\n\tprint correlation coefficient of distance for mutating simulation (eucledian, eSDC)\n\n";
    cout << "----Parameter setting----\n\n"
         << "--constraint [num]\n\tset the maximal span to num\n"
         << "-s [num]\n\tstart point of base pairing probability (1~(sequence length-1)) \n"
         << "-e [num]\n\tend point of base pairing probability\n\t\tif you assign only a start position for one sequence, it'll be the sequence length.\n"
         << "-w [num] (--window [num])\n\twindow size for accessibility (num+1 will be applied).\n"
         << "-T [temperature]\n\tchange temperature (not completed!)\n\n"
         << "--energy [.par file]\n\talso possible to use \"Turner2004(old)\", \"Andronescu\", and \"Turner1999\" as an abbreviation\n\n"
         << "--hard \"[.*()]\"\n\thard constraint setting (only valid with --pre option), or compute the structure set following the hard constraint. (.: loop, (): any stem, *: any)\n"
         << "\t\t(interaction mode: AAANNNUUU --hard **(nnn*)* -> compute interaction energy of AAA and UUU with hairpin loop including NNN (without hairpin energy)\n"
         << "--inter\tto compute the interaction energy between two RNAs ignoring internal base pairs\n\n";
    cout << "------I/O setting------\n\n"
         << "-r\n\tuse complementary sequence\n"
         << "-f [sequence]\n\tdirect input of sequence\n"
         << "--input [filename]\n\tdeal filename as sequence file input\n"
         << "--name [name]\n\tprefix of db filename\n"
         << "-t\n\tkeep temporary douter files after connecting (only valid with connect option)\n\n"
         << "--cd\n\t!!!warning!!! use another directory via a relative path. (Please confirm existence of outer and prob directory.)\n"
         << "--stdout\n\ttext output mode (only valid with stemdb option)\n"
         << "--init_file\n\tremove temp files for connect procedure (only valid with --connect and --save_memory option)\n";
         // << "--remote [input]\tcalculate the stability of remote duplexes (not completed)\n"
         // << "--transcribed\tsimulate the arrangement of structure during transcription (not completed)\n"
}

struct option* option()
{
    struct option *options = new struct option[OPTION+1]();
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
    options[14].name = "save_memory";
    options[15].name = "init_file";
    options[16].name = "acc";
    options[17].name = "prof";
    options[18].name = "motif";
    options[19].name = "remote";
    options[20].name = "transcribed";
    options[21].name = "mout";
    options[22].name = "stdout";
    options[23].name = "cd";
    options[24].name = "boundary";
    options[25].name = "entropy";
    options[26].name = "mfe";
    options[27].name = "hard";
    options[28].name = "window";
    options[29].name = "inter";
    options[30].name = "help";

    for (int i = 0; i < OPTION; i++) {
        switch(i) {
            case 4: case 5: case 6: case 9: case 11: case 12: case 13: case 14:
            case 15: case 16: case 17: case 18: case 20: case 22: case 23: case 24: case 25: case 26: case 29: case 30:
                options[i].has_arg = 0;
                break;
            case 8: case 10: case 21:
                options[i].has_arg = optional_argument;
                break;
            default:
               options[i].has_arg = required_argument;
               break;
        }
        options[i].flag = NULL; options[i].val = 1;
    }
    options[OPTION].name = 0; options[OPTION].has_arg = 0;
    options[OPTION].flag = 0; options[OPTION].val = 0;
    return options;
}

bool SetArg(int option_index, const char* optarg, Rfold::Arg& arg)
{
    switch(option_index) {
        case 0:
            if (optarg) arg.constraint = atoi(optarg);
            break;
        case 1:
            if (optarg) arg.input = string(optarg);
            break;
        case 2:
            if (optarg)
                ;
            // arg.outer_input = string(optarg);
            // arg.init_calc = Rfold::Arg::Calc::Bpp;
            break;
        case 3: arg.name = string(optarg); break;
        case 4: arg.init_calc = Rfold::Arg::Calc::Divide; break;
        case 5: arg.init_calc = Rfold::Arg::Calc::Connect; break;
        case 6: arg.init_calc = Rfold::Arg::Calc::Stemdb; break;
        case 7: arg.ene = string(optarg); break;
        case 8:
            if (optarg) arg.minp = atof(optarg);
            if (arg.init_calc != Rfold::Arg::Calc::Stemdb)
                arg.init_calc = Rfold::Arg::Calc::Bpp;
            break;
        case 9: arg.text = true; break;
        case 10:
            if (optarg) arg.gamma = atof(optarg);
            arg.mea_flag = true;
            if (arg.init_calc != Rfold::Arg::Calc::Stemdb)
                arg.init_calc = Rfold::Arg::Calc::Bpp;
            break;
        case 11: arg.image = true; break;
        case 12:
            if (arg.init_calc != Rfold::Arg::Calc::Stemdb)
                arg.init_calc = Rfold::Arg::Calc::Bpp;
            arg.stem_flag = true;
            break;
        case 13: arg.pre_flag = true; break;
        case 14: arg.save_memory = true; break;
        case 15: arg.init_file = true; break;
        case 16: arg.acc_flag = true; break;
        case 17: arg.prof_flag = arg.acc_flag = true; break;
        case 18: arg.prof_flag = true; break;
        case 19: arg.init_calc = 0; arg.input = string(optarg); break;
        case 20: arg.init_calc = 0; break;
        case 21:
            if (optarg) arg.mout = string(optarg);
            arg.mout_flag = true; break;
        case 22: arg.outtext = true; break;
        case 23: arg.cd = true; break;
        case 24:
            arg.boundary = true;
            if (arg.init_calc != Rfold::Arg::Calc::Stemdb)
                arg.init_calc = Rfold::Arg::Calc::Bpp;
            arg.stem_flag = true; break;
        case 25:
            arg.entro_flag = true;
            if (arg.init_calc != Rfold::Arg::Calc::Stemdb)
                arg.init_calc = Rfold::Arg::Calc::Bpp;
            break;
        case 26:
            arg.mfe_flag = true; break;
        case 27:
            arg.hard_const = string(optarg); break;
        case 28:
            arg.window = atoi(optarg); break;
        case 29:
            arg.inter_flag = true; break;
        default:
            PrintDetailedHelpOption();
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


void RNATransform(Rfold::Arg& arg, string& seq)
{
    Rfold::Parameter::GetCapitalRNA(seq);
    if (arg.compl_flag) {
        arg.name += "_rev_";
        Rfold::Parameter::GetCompCapitalRNA(seq);
    }
}

void RNATransform(Rfold::Arg& arg) {
    RNATransform(arg, arg.str);
}

bool NameTransform(string& name, bool cd) {
    string rep = "";
    if (name.length() == 0) return true;
    if (!cd) {
        for (string::size_type pos = name.find("/"); pos != string::npos; pos = name.find("/", rep.length()+pos))
            name.replace(pos, 1, rep);
        for (string::size_type pos = name.find("\\"); pos != string::npos; pos = name.find("\\", rep.length()+pos))
            name.replace(pos, 1, rep);
    }
    cout << "#-Check file prefix: " << name << endl;
    if (cd) {
        string::size_type pos = name.find_last_of("/");
        if (pos == string::npos) pos = name.find_last_of("\\");
        if (pos == string::npos) return true;
        string dir = HOMEDIR+name.substr(0, pos);
        struct stat buf;
        if (stat(dir.c_str(), &buf) != 0 || strlen(dir.c_str()) < MINDIRLEN) {
            cerr << "Cannot find working directory or may contain incorrect (too short) path.\n"
                << "Please make sure that the directory (" << dir << ") exists." << endl;
            return false;
        }
    }
    return true;
}

bool InitCondition(Rfold::Arg& arg)
{
    RNATransform(arg);
    if (!NameTransform(arg.name, arg.cd))
        return false;
    if (arg.save_memory && arg.compl_flag) {
        cerr << "Please do not use --save_memory and -r option at same time." << endl;
        return false;
    } else if (arg.id >= 0 && arg.init_calc == Rfold::Arg::Calc::Pre)
        arg.init_calc = Rfold::Arg::Calc::Divide;
    if (arg.id == arg.chunk) {
        if (arg.init_calc == Rfold::Arg::Calc::Divide)
            arg.init_calc = Rfold::Arg::Calc::Connect;
    } else if (arg.id > arg.chunk) {
        cerr << "id number is too large!" << endl;
        return false;
    } else if (arg.id < 0) {
        if (arg.init_file && arg.init_calc == Rfold::Arg::Calc::Connect)
            return true;
        else if (arg.pre_flag || arg.init_calc != Rfold::Arg::Calc::Bpp)
            arg.init_calc = Rfold::Arg::Calc::Pre;
    }
    return true;
}

void CalcOneSeq(Rfold::Arg& arg)
{
    if ((LEN)arg.str.length() == 0 && arg.length == 0) return;
    if (!InitCondition(arg)) return;
    switch(arg.init_calc) {
        case Rfold::Arg::Calc::Divide:
            if (arg.id >= 0) {
                cout << "# Dividing (id: " << arg.id  << "/" << arg.chunk << ")" << endl;
                Rfold::ParasoR::DivideChunk(arg);
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
        // case Rfold::Arg::Calc::Remote:
        //     Rfold::ParasoR::Remote(arg);
        //     break;
        // case Rfold::Arg::Calc::Transcribed:
        //     Rfold::ParasoR::Transcribed(arg);
        //     break;
    }
}

void CalcStrucFasta(Rfold::Arg& arg)
{
    string str;
    string tname = arg.name;
    ifstream ifs(arg.input.c_str());
    cout << "#-File Reading " << arg.input << "..." << endl;
    while (getline(ifs, str)) {
        if (str.length() == 0) continue;
        Rfold::EraseWindowsNewline(str);
        if (str[0] == '>') {
            CalcOneSeq(arg);
            arg.name = tname+str.substr(1);
            if (arg.save_memory) {
                arg.length = static_cast<LEN>(0);
                arg.seqID++;
            } else {
                arg.str = "";
            }
        } else {
            if (arg.save_memory) {
                arg.length += static_cast<LEN>(str.length());
            } else {
                arg.str += str;
            }
        }
    }
    if (arg.str != "" || arg.length > 0)
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
        cout << "#-Test with an example sequence: " << arg.str << endl;
        if (arg.start >= 0 && arg.end < 0) arg.end = (int)arg.str.length();
        if (arg.name.length() == 0)
            arg.name = "Sample0_";
        CalcOneSeq(arg);
    }
}

int main(int argc, char** argv)
{
    int option_index = 0;
    Rfold::Arg arg = Rfold::Arg();
    try {
        struct option *options = option();
        bool help_flag = true;
        while (1) {
            int opt = getopt_long(argc, argv, "m:w:s:e:api:k:f:cdrtT:", options, &option_index);
            if (opt == -1) break;
            help_flag = false;
            switch(opt) {
                case 0: case 1:
                    if (!SetArg(option_index, optarg, arg)) {
                        delete[] options;
                        return 0;
                    }
                    break;
                case 's': if (optarg != NULL) arg.start = atoi(optarg)-1; break;
                case 'e': if (optarg != NULL) arg.end = atoi(optarg); break;
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
        cout << "#-Check working directory: " << HOMEDIR << endl;
        struct stat buf;
        if (stat(HOMEDIR, &buf) != 0 || strlen(HOMEDIR) < MINDIRLEN) {
            cerr << "Cannot find working directory or may contain incorrect (too short) path.\n"
                 << "Please execute below commands and recompile ParasoR.\n"
                 << "  make clean; make\n";
            return 1;
        }
        if (help_flag) {
            PrintHelpOption();
        } else {
            Rfold::Parameter::SetTemperature(arg.temp);
            Rfold::Parameter::ChangeEnergyParam(arg.ene);
            if (Rfold::Parameter::initialized)
                CheckCondition(arg);
        }
    } catch (const char* str) {
        cout << "Catch error*" << endl;
        cout << str << endl;
    }
    return 0;
}
