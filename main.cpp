#include <fstream>
#include <iostream>
#include<memory>
#include<chrono>
#include<vector>

using namespace std;
//--------------------------------------------------------------------------
class Tlogger{
public:
    virtual void print(size_t tiks_start,string b)=0;
};
//--------------------------------------------------------------------------
class Tlogger_to_file: public Tlogger{
    static const string title_file_name;
    void print(size_t tiks_start,string b){
        string file_name = title_file_name+ to_string(tiks_start)+".log";
        ofstream file_(file_name);
        if(file_.is_open()){
            file_ << b;
            file_.close();
        }
    }
};
const string Tlogger_to_file::title_file_name = "bulk";
//--------------------------------------------------------------------------
template<class Tlog = Tlogger_to_file>
class Tpoket_{
    static unique_ptr<Tlogger> logger;
    static const string title;
    vector<string> bulk;
    size_t tiks_start;
    bool brace;
    public:
    Tpoket_():bulk(){
       if(!logger){
            logger = make_unique<Tlog>();
       }
    }
    bool empty_poket()const{
        return bulk.empty();
    }
    void add_cmd(string cmd){
        if(empty_poket()){
            auto now = chrono::system_clock::now();
            tiks_start = chrono::duration_cast<chrono::system_clock::duration>(now.time_since_epoch()).count();
        }
        if(cmd.empty())return;
        bulk.push_back(cmd);
    }
    string bulk_to_string()const {
        string out = title;
        if(bulk.empty()) return out;
        size_t i = 0;
        for(;i < bulk.size()-1;i++){out+=bulk[i]+",";}
        out +=bulk[i];
        return out;
    }
    const Tpoket_& print_poket() const{
        if(empty_poket())return *this;
        cout << bulk_to_string() << "\n";
        return *this;
    }
    const Tpoket_& log_poket()const {
        if(empty_poket())return *this;
        logger->print(tiks_start,bulk_to_string());
        return *this;
    }
};
template<class Tlog>
unique_ptr<Tlogger>  Tpoket_<Tlog>::logger = nullptr;
template<class Tlog>
const string Tpoket_<Tlog>::title = "bulk: ";
using Tpoket = Tpoket_<>;
//--------------------------------------------------------------------------
class Tparser_cmd{
    enum{OPEN_BRACE=1,CLOSE_BRACE=0};
    static const string brace_l;
    static const string brace_r;
    static const string delimetr;
    int N;
    unique_ptr<Tpoket> current_poket;
    public:
    Tparser_cmd(int N):N(N){
        current_poket = make_unique<Tpoket>();
    }
    unique_ptr<Tpoket> operator()(string& cmd);
    unique_ptr<Tpoket> return_out_poket(int& N_,string& cmd);
};
const string Tparser_cmd::brace_l = "{";
const string Tparser_cmd::brace_r = "}";
const string Tparser_cmd::delimetr = ",";
unique_ptr<Tpoket> Tparser_cmd::return_out_poket(int& N_,string& cmd){
    N_ = N;
    current_poket->add_cmd(cmd);
    unique_ptr<Tpoket> out = move(current_poket);
    current_poket = make_unique<Tpoket>();
    return out ;
}
unique_ptr<Tpoket> Tparser_cmd::operator()(string& cmd){
        static int N_ = N;
        static int counter_brace = 0;
        if(cmd==brace_l){
            counter_brace++;
            if(counter_brace==OPEN_BRACE){return return_out_poket(N_,cmd="");}
            return make_unique<Tpoket>();
        }
        if(cmd==brace_r){
            counter_brace--;
            if(counter_brace==CLOSE_BRACE){return return_out_poket(N_,cmd="");}
            return make_unique<Tpoket>();
        }
        if(counter_brace==0){ N_--;}
        if(!N_){return return_out_poket(N_,cmd);}
        current_poket->add_cmd(cmd);
        return make_unique<Tpoket>();
}
//--------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    if(argc == 1){return 0;}
    unique_ptr <Tparser_cmd> parser_cmd =  make_unique<Tparser_cmd>( atoi(argv[1]));
    string cmd;
    while (getline(cin, cmd) && !cin.eof()) {
        (*(*parser_cmd)(cmd)).print_poket().log_poket();
    }
    (*(*parser_cmd)(cmd)).print_poket().log_poket();
    return 0;
}
