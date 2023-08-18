#include "scanner.hh"
#include "parser.tab.hh"
#include <fstream>
using namespace std;
static string tab = "	";

SymbTab gst, gstfun, gststruct;
string filename;
std::vector<string> lc;
extern std::map<string, abstract_astnode *> ast;
std::map<std::string, datatype> predefined{
	{"printf", createtype(VOID_TYPE)},
	{"scanf", createtype(VOID_TYPE)},
	{"mod", createtype(INT_TYPE)}};
int main(int argc, char **argv)
{
	fstream in_file, out_file;

	in_file.open(argv[1], ios::in);

	IPL::Scanner scanner(in_file);

	IPL::Parser parser(scanner);

#ifdef YYDEBUG
	parser.set_debug_level(1);
#endif
	parser.parse();
	// create gstfun with function entries only

	for (const auto &entry : gst.Entries)
	{
		if (entry.second.varfun == "fun")
			gstfun.Entries.insert({entry.first, entry.second});
	}
	// create gststruct with struct entries only

	for (const auto &entry : gst.Entries)
	{
		if (entry.second.varfun == "struct")
			gststruct.Entries.insert({entry.first, entry.second});
	}

	cout << tab << ".file \"" << argv[1] << "\"" << endl;  
	cout << tab << ".text" << endl;

	for (int i = 0; i < (int)lc.size(); i++)
	{
		cout << lc[i] << endl;
	}

	for (auto it = gstfun.Entries.begin(); it != gstfun.Entries.end(); ++it)
	{
		cout << tab<< ".globl " << it->first <<endl<<tab<< ".type" << tab << it->first << ", @function" << endl;
		int local_size = 0;
		for (const auto &it2 : it->second.symbtab->Entries)
		{
			if (it2.second.scope == "local")
			{
				local_size += it2.second.size;
			}
		}
		cout << it->first << ":" << endl;
		cout << tab << "pushl %ebp"<<endl<<tab<<"movl %esp, %ebp" << endl;
		cout << tab << "subl $" << local_size << ", %esp" << endl;
		ast[it->first]->print(0);
		cout << tab << "addl $" << local_size << ", %esp" << endl;
		cout << tab << "leave"<<endl<<tab<<"ret"<<endl<<tab<<".size	" << it->first << ", .-" << it->first << endl;
	}

	cout << ".ident	\"GCC: (Ubuntu 8.1.0-9ubuntu1~16.04.york1) 8.1.0\" "<<endl<<".section	.note.GNU-stack,\"\",@progbits" << endl;
	cout << ".section	.note.gnu.property,\"a\""<<endl;
	cout << ".align 4"<<endl;
	cout << ".long	 1f - 0f"<<endl;
	cout << ".long	 4f - 1f"<<endl;
	cout << ".long	 5"<<endl;
	cout << "0:"<<endl;
	cout << tab << ".string	 \"GNU\""<<endl;
	cout << "1:"<<endl;
	cout << tab << ".align 4"<<endl;
	cout << tab << ".long	 0xc0000002"<<endl;
	cout << tab << ".long	 3f - 2f"<<endl;
	cout << "2:"<<endl;
	cout << tab << ".long	 0x3"<<endl;
	cout << "3:"<<endl;
	cout << tab << ".align 4"<<endl;
	cout << "4:"<<endl;

	fclose(stdout);
}