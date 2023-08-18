#include "ast.hh"
#include <cstdarg>
#include "symbtab.hh"

extern SymbTab gst;

vector<string> regstk {"%edi", "%esi", "%edx", "%ecx", "%ebx", "%eax"}; //  edi, esi, edx, ecx, ebx, eax
int  counterl = 2;
static string tab = "	";

bool rakesh = false;

// Define a helper function to generate a unique node_count
string generatenode_count() {
   
    return ".L" + to_string(counterl++);
}

// Perform the comparison operation and update the result in the regstk
void performComparison(const string& op, const string& lhs, const string& rhs) {
    string truenode_count = generatenode_count();
    string falsenode_count = generatenode_count();
    string endnode_count = generatenode_count();

    if (op == "LT_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "jl " << truenode_count << endl;
    } else if (op == "GT_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "jg " << truenode_count << endl;
    } else if (op == "LE_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "jle " << truenode_count << endl;
    } else if (op == "GE_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "jge " << truenode_count << endl;
    } else if (op == "EQ_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "je " << truenode_count << endl;
    } else if (op == "NE_OP_INT") {
        cout << "cmpl " << rhs << ", " << lhs << endl;
        cout << "jne " << truenode_count << endl;
    }

    // False case
    cout << "movl $0, " << lhs << endl;
    cout << "jmp " << endnode_count << endl;

    // True case
    cout << truenode_count << ":" << endl;
    cout << "movl $1, " << lhs << endl;

    // End node_count
    cout << endnode_count << ":" << endl;
}

// Generate code for logical AND operation
void generateLogicalAnd(const string& lhs, const string& rhs) {
    string falsenode_count = generatenode_count();
    string endnode_count = generatenode_count();

    // Short-circuit evaluation
    cout << "cmpl $0, " << lhs << endl;
    cout << "je " << falsenode_count << endl;
    cout << "cmpl $0, " << rhs << endl;
    cout << "je " << falsenode_count << endl;

    // Both operands are true, set result to 1
    cout << "movl $1, " << lhs << endl;
    cout << "jmp " << endnode_count << endl;

    // False node_count
    cout << falsenode_count << ":" << endl;
    cout << "movl $0, " << lhs << endl;

    // End node_count
    cout << endnode_count << ":" << endl;
}

// Generate code for logical OR operation
void generateLogicalOr(const string& lhs, const string& rhs) {
    string truenode_count = generatenode_count();
    string endnode_count = generatenode_count();

    // Short-circuit evaluation
    cout << "cmpl $0, " << lhs << endl;
    cout << "jne " << truenode_count << endl;
    cout << "cmpl $0, " << rhs << endl;
    cout << "jne " << truenode_count << endl;

    // Both operands are false, set result to 0
    cout << "movl $0, " << lhs << endl;
    cout << "jmp " << endnode_count << endl;

    // True node_count
    cout << truenode_count << ":" << endl;
    cout << "movl $1, " << lhs << endl;

    // End node_count
    cout << endnode_count << ":" << endl;
}

// Generate code for binary expressions
void generateBinaryExpression(const string& op, const string& lhs, const string& rhs, bool check, int structsize) {
    if (op == "PLUS_INT") {
		if(check){
			cout << "imul $"<< structsize <<", " << rhs <<endl;
		}
        cout << "addl " << rhs << ", " << lhs << endl;
    } else if (op == "MINUS_INT") {
		if(check){
			cout << "imul $"<< structsize <<", " << rhs <<endl;
		}
        cout << "subl " << rhs << ", " << lhs << endl;
    } else if (op == "MULT_INT") {
        cout << "imul " << rhs << ", " << lhs << endl;
    }
}

void genCode(exp_astnode* l, exp_astnode* r, string op)
{
	if(op == "="){
				std::string temp = regstk.back();
				regstk.back() = regstk[regstk.size() - 2];
				regstk[regstk.size() - 2] = temp;

				std::string output;

				r->print(0); 
				if (r->astnode_type == ArrayRefNode || r->astnode_type == MemberNode)
				{
					output.append("    ").append("movl (").append(regstk.back()).append("), ").append(regstk.back()).append("\n");
				}

				std::string R = regstk.back();
				regstk.pop_back();

				if (l->astnode_type == ArrowNode)
				{
					output.append(tab).append("movl ").append(std::to_string(l->offset)).append("(%ebp),").append(regstk.back()).append("\n");
					output.append(tab).append("movl ").append(R).append(", ").append(std::to_string(l->offset2)).append("(").append(regstk.back()).append(")").append("\n");
				}
				else if (l->astnode_type == OpUnaryNode && l->nameofnode == "DEREF")
				{
					output.append(tab).append("movl ").append(std::to_string(l->offset)).append("(%ebp),").append(regstk.back()).append("\n");
					output.append(tab).append("movl ").append(R).append(", (").append(regstk.back()).append(") ").append("\n");
				}
				else if (l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode)
				{
					l->print(0);
					output.append("    ").append("movl ").append(R).append(", (").append(regstk.back()).append(")").append("\n");
				}
				else
				{
					output.append(tab).append("movl ").append(R).append(", ").append(std::to_string(l->offset)).append("(%ebp)").append("\n");
				}

				regstk.push_back(R);

				temp = regstk.back();
				regstk.back() = regstk[regstk.size() - 2];
				regstk[regstk.size() - 2] = temp;

				std::cout << output;

				}

	else if(l->node_count < (int)regstk.size() && l->node_count < r->node_count)
	{
		string temp = regstk.back();
		regstk.back() = regstk[regstk.size()-2];
		regstk[regstk.size()-2] = temp;

		r->print(0);
		if(r->astnode_type == ArrayRefNode || r->astnode_type == MemberNode)
		{
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}

		string R = regstk.back();
		regstk.pop_back();

		if(op == "DIV_INT")
		{

			if(R == "%eax")
			{
				cout << tab << "pushl %ecx " << endl;
				cout << tab << "movl " << regstk.back() << ", " << "%ecx" << endl;
				cout << tab << "cltd" << endl;
				cout << tab << "idivl %ecx" << endl;
				cout << tab << "popl %ecx " << endl;
			}
			else
			{
				cout << tab << "pushl %eax " << endl;
				cout << tab << "pushl %ecx " << endl;

				cout << tab << "movl " << regstk.back() << ", " << "%ecx" << endl;

				cout << tab << "pushl %ecx " << endl;
				l->print(0);
				if(l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode)
				{
					cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
				}
				cout << tab << "movl " << regstk.back() << ", " << "%eax" << endl;  
				cout << tab << "popl %ecx " << endl;

				cout << tab << "cltd" << endl;
				cout << tab << "idivl %ecx" << endl;
				cout << tab << "movl " << "%eax" << ", " << R << endl;  
				cout << tab << "popl %ecx " << endl;
				cout << tab << "popl %eax " << endl;
			}
			regstk.push_back(R);

		temp = regstk.back();
		regstk.back() = regstk[regstk.size()-2];
		regstk[regstk.size()-2] = temp;

			return;
		}

		l->print(0);
		if(l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode){
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}
		if (op == "AND_OP") {
			generateLogicalAnd(regstk.back(), R);
		} else if (op == "OR_OP") {
			generateLogicalOr(regstk.back(), R);
		} else if(op == "LT_OP_INT" || op == "GT_OP_INT" || op == "GE_OP_INT" || op == "LE_OP_INT" || op == "EQ_OP_INT" || op == "NE_OP_INT"){
			performComparison(op, regstk.back(), R);
		} else {
			int size = 4;
			if (l->data_type.structname != "")
				size = gst.Entries[l->data_type.structname].size;
			generateBinaryExpression(op, regstk.back(), R,  l->data_type.deref || isArray(l->data_type), size);
		}

		regstk.push_back(R);

		temp = regstk.back();
		regstk.back() = regstk[regstk.size()-2];
		regstk[regstk.size()-2] = temp;

	}

	else if(r->node_count < (int)regstk.size() && l->node_count >= r->node_count)
	{
		l->print(0);
		
		if(l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode){
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}
		
		string R = regstk.back();
		regstk.pop_back();

		r->print(0);
		if(r->astnode_type == ArrayRefNode || r->astnode_type == MemberNode)
		{
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}
		if(op == "DIV_INT")
		{
			if(R == "%eax")
			{
				cout << tab << "pushl %ecx " << endl;
				cout << tab << "movl " << regstk.back() << ", " << "%ecx" << endl;
				cout << tab << "cltd" << endl;
				cout << tab << "idivl %ecx" << endl;
				cout << tab << "popl %ecx " << endl;
			}
			else
			{
				cout << tab << "pushl %eax " << endl;
				cout << tab << "pushl %ecx " << endl;

				cout << tab << "movl " << regstk.back() << ", " << "%ecx" << endl;

				cout << tab << "pushl %ecx " << endl;
				l->print(0);
				if(l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode)
				{
					cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
				}
				cout << tab << "movl " << regstk.back() << ", " << "%eax" << endl;  
				cout << tab << "popl %ecx " << endl;

				cout << tab << "cltd" << endl;
				cout << tab << "idivl %ecx" << endl;
				cout << tab << "movl " << "%eax" << ", " << R << endl;  
				cout << tab << "popl %ecx " << endl;
				cout << tab << "popl %eax " << endl;
			}
		}
		else if (op == "AND_OP") {
			generateLogicalAnd(R, regstk.back());
		} else if (op == "OR_OP") {
			generateLogicalOr(R, regstk.back());
		} else if(op == "LT_OP_INT" || op == "GT_OP_INT" || op == "GE_OP_INT" || op == "LE_OP_INT" || op == "EQ_OP_INT" || op == "NE_OP_INT"){
			performComparison(op, R, regstk.back());
		} else {
			int size = 4;
			if (l->data_type.structname != "")
				size = gst.Entries[l->data_type.structname].size;
			generateBinaryExpression(op, R, regstk.back(),  l->data_type.deref || isArray(l->data_type), size);
		}

		regstk.push_back(R);
	}

	else if (l->node_count >= (int)regstk.size() && r->node_count >= (int)regstk.size())  
	{	
		r->print(0);
		if(r->astnode_type == ArrayRefNode)
		{
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}
		cout << tab << "pushl " << regstk.back() << endl;

		if(op == "=")
		{
			cout << tab << "movl " << regstk.back() << ", " << l->offset << "(%ebp)" << endl;
			cout << tab << "addl " << "$4" << ", " << "%esp" << endl;
			return;
		}
		l->print(0);
		if(l->astnode_type == ArrayRefNode || l->astnode_type == MemberNode)
		{
			cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
		}
		if (op == "AND_OP") {
			generateLogicalAnd(regstk.back(), "(%esp)");
			cout << tab << "addl " << "$4" << ", " << "%esp" << endl;
		} else if (op == "OR_OP") {
			generateLogicalOr(regstk.back(), "(%esp)");
			cout << tab << "addl " << "$4" << ", " << "%esp" << endl;
		} else if(op == "LT_OP_INT" || op == "GT_OP_INT" || op == "GE_OP_INT" || op == "LE_OP_INT" || op == "EQ_OP_INT" || op == "NE_OP_INT"){
			performComparison(op, regstk.back(), "(%esp)");
			cout << tab << "addl " << "$4" << ", " << "%esp" << endl;
		} else {
			int size = 4;
			if (l->data_type.structname != "")
				size = gst.Entries[l->data_type.structname].size;
			generateBinaryExpression(op, regstk.back(), "(%esp)", l->data_type.deref, size);
			cout << tab << "addl " << "$4" << ", " << "%esp" << endl;
		}

	}
	
}
/////////////////////////////

empty_astnode::empty_astnode() : statement_astnode()
{
	astnode_type = EmptyNode;
}

void empty_astnode::print(int ntabs)
{
	;
}

//////////////////////////

seq_astnode::seq_astnode() : statement_astnode()
{

	astnode_type = SeqNode;
}

void seq_astnode::pushback(statement_astnode *child)
{
	children_nodes.push_back(child);
}

void seq_astnode::print(int ntabs)
{
	printblanks(ntabs);
	printAst("", "l", "seq", &children_nodes);
}

///////////////////////////////////

assignS_astnode::assignS_astnode(exp_astnode *l, exp_astnode *r, string tc) : statement_astnode()
{
	typecast = tc;
	left = l;
	right = r;
	id = "Ass";
	astnode_type = AssNode;
}

void assignS_astnode::print(int ntabs)
{
    genCode(left, right, "=");
}

///////////////////////////////////

return_astnode::return_astnode(exp_astnode *c, int size) : statement_astnode()
{
	child = c;
	id = "Return";
	astnode_type = ReturnNode;
	param_size = size;
}
void return_astnode::print(int ntabs)
{
	child->print(0);
	if(child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode)
	{
		cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
	}

	cout << tab << "movl " << regstk.back() << ", %ebx" << endl;
	cout << tab << "movl %ebx, %ebx" << endl;
	cout << tab << "leave\n    ret\n  "  << endl;
}

////////////////////////////////////

if_astnode::if_astnode(exp_astnode *l, statement_astnode *m, statement_astnode *r) : statement_astnode()
{
	left = l;
	middle = m;
	right = r;
	id = "If";
	astnode_type = IfNode;
}

void if_astnode::print(int ntabs)
{
	int a =  counterl;
	 counterl += 2;
	left->print(0);
	cout << tab << "cmpl $0, " << regstk.back() << endl;
	cout << tab << "je  .L" << a << endl;
	middle->print(0);
	cout << tab << "jmp  .L" << a+1 << endl;
	cout << ".L" << a << " :" << endl;
	right->print(0);
	cout << ".L" << a+1 << " :" << endl;
}
////////////////////////////////////

while_astnode::while_astnode(exp_astnode *l, statement_astnode *r) : statement_astnode()
{
	left = l;
	right = r;
	id = "While";
	astnode_type = WhileNode;
}

void while_astnode::print(int ntabs)
{
	int a =  counterl;
	 counterl += 2;
	cout << tab << "jmp  .L" << a << endl;
	cout << ".L" << a+1 << " :" << endl;
	right->print(0);
	cout << ".L" << a << " :" << endl;
	left->print(0);
	cout << tab << "cmpl $1, " << regstk.back() << endl;
	cout << tab << "je  .L" << a+1 << endl;
}
/////////////////////////////////

for_astnode::for_astnode(exp_astnode *l, exp_astnode *m1, exp_astnode *m2, statement_astnode *r) : statement_astnode()
{
	left = l;
	middle1 = m1;
	middle2 = m2;
	right = r;
	id = "For";
	astnode_type = ForNode;
}

void for_astnode::print(int ntabs)
{
	int a =  counterl;
	 counterl += 2;
	left->print(0);
	cout << tab << "jmp  .L" << a << endl;
	cout << ".L" << a+1 << " :" << endl;
	right->print(0);
	middle2->print(0);
	cout << ".L" << a << " :" << endl;
	middle1->print(0);
	cout << tab << "cmpl $1, " << regstk.back() << endl;
	cout << tab << "je  .L" << a+1 << endl;
}

//////////////////////////////////

// exp_astnode::exp_astnode() : abstract_astnode()
// {
// }

//////////////////////////////////
string exp_astnode::idname()
{
	return id;
};

op_binary_astnode::op_binary_astnode(string val, exp_astnode *l, exp_astnode *r) : exp_astnode()
{
	id = val;
	nameofnode = val;
	left = l;
	right = r;
	astnode_type = OpBinaryNode;
	if(l->node_count == r->node_count)
	{
		node_count = l->node_count + 1;
	}
	else
	{
		node_count = max(l->node_count, r->node_count);
	}
}

void op_binary_astnode::print(int ntabs)
{
	genCode(left, right, id);
}

///////////////////////////////////

op_unary_astnode::op_unary_astnode(string val) : exp_astnode()
{
	id = val;
	astnode_type = OpUnaryNode;
}

void op_unary_astnode::print(int ntabs)

						{
							if (nameofnode == "PP")
							{
								std::string reg1 = regstk.back();
								std::string reg2 = regstk[regstk.size() - 2];
								std::string output = tab;
								output.append("movl ").append(to_string(child->offset)).append("(%ebp),").append(reg1).append("\n");
								output.append(tab).append("leal 1(").append(reg1).append("), ").append(reg2).append("\n");
								output.append(tab).append("movl ").append(reg2).append(", ").append(to_string(child->offset)).append("(%ebp)").append("\n");
								std::cout << output;
							}
							else if (nameofnode == "UMINUS")
							{
								child->print(0);
								std::string reg = regstk.back();
								std::string output;
								if (child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode)
								{
									output.append("    ").append("movl (").append(reg).append("), ").append(reg).append("\n");
								}
								output.append(tab).append("negl ").append(reg).append("\n");
								std::cout << output;
							}
							else if (nameofnode == "NOT")
							{
								child->print(0);
								std::string reg = regstk.back();
								std::string output;
								if (child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode)
								{
									output.append("    ").append("movl (").append(reg).append("), ").append(reg).append("\n");
								}
								output.append(tab).append("cmpl $0, ").append(reg).append("\n");
								output.append(tab).append("je  .L").append(std::to_string(counterl)).append("\n");
								output.append(tab).append("movl $0, ").append(reg).append("\n");
								counterl++;
								output.append(tab).append("jmp  .L").append(std::to_string(counterl)).append("\n");
								output.append(".L").append(std::to_string(counterl - 1)).append(" :\n");
								output.append(tab).append("movl $1, ").append(reg).append("\n");
								output.append(".L").append(std::to_string(counterl)).append(" :\n");
								counterl++;
								std::cout << output;
							}
							else if (nameofnode == "ADDRESS")
							{
								std::string reg = regstk.back();
								std::string output = tab;
								output.append("leal ").append(to_string(child->offset)).append("(%ebp),").append(reg).append("\n");
								std::cout << output;
							}
							else if (nameofnode == "DEREF")
							{
								child->print(0);
								std::string reg = regstk.back();
								std::string output;
								if (child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode)
								{
									output.append("    ").append("movl (").append(reg).append("), ").append(reg).append("\n");
								}
								output.append(tab).append("movl (").append(reg).append("), ").append(reg).append("\n");
								std::cout << output;
							}

}

op_unary_astnode::op_unary_astnode(string val, exp_astnode *l) : exp_astnode()
{
	id = val;
	nameofnode = val;
	child = l;
	astnode_type = OpUnaryNode;
	offset = l->offset;
	node_count = l->node_count;
}

string op_unary_astnode::getoperator()
{
	return id;
}
///////////////////////////////////

assignE_astnode::assignE_astnode(exp_astnode *l, exp_astnode *r) : exp_astnode()
{
	left = l;
	right = r;
	astnode_type = AssignNode;
}

void assignE_astnode::print(int ntabs)
{
	genCode(left, right, "=");
}

///////////////////////////////////

funcall_astnode::funcall_astnode() : exp_astnode()
{
	astnode_type = FunCallNode;
}

funcall_astnode::funcall_astnode(identifier_astnode *child)
{
	funcname = child;
	astnode_type = FunCallNode;
}

void funcall_astnode::setname(string name)
{
	funcname = new identifier_astnode(name);
}

void funcall_astnode::pushback(exp_astnode *subtree)
{
	children.push_back(subtree);
}

void funcall_astnode::print(int ntabs)
{
	// New code implementation

	// Preserve registers by pushing them onto the regstk
	string registers[] = {"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"};
	for (const string& reg : registers) {
		if (reg != regstk.back()) {
			cout << tab << "pushl " << reg << endl;
		}
	}

	// Push arguments onto the regstk
	for (const auto& child : children) {
		if (child->data_type.type == 3) {
			for (int j = 0; j < child->data_type.size() / 4; j++) {
				int offset = child->offset + child->data_type.size() - 4 * (j + 1);
				cout << tab << "pushl " << offset << "(%ebp)" << endl;
			}
		} 
		else {
			child->print(0);
			if(child->data_type.array.size() == 0)
			{
				if(child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode){
					cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
				}
			}

			cout << tab << "pushl " << regstk.back() << endl;
		}
	}

	// Call the function
	cout << tab << "call " << funcname->nameofnode << endl;

	// Adjust the regstk pointer after the function call
	for (const auto& child : children) {
	cout << tab << "addl $" << child->data_type.size() << ", %esp" << endl;
	}

	cout << "movl %eax, " << regstk.back() << endl;  

	// Restore registers by popping them from the regstk in reverse order
	for (int i = sizeof(registers) / sizeof(registers[0]) - 1; i >= 0; i--) {
		if (registers[i] != regstk.back()) {
			cout << tab << "pop " << registers[i] << endl;
		}
	}



}

proccall_astnode::proccall_astnode(funcall_astnode *fc)
{
	procname = fc->funcname;
	children = fc->children;
	printf_counter = fc->printf_counter;	
}

void proccall_astnode::print(int ntabs)
{
	if(procname->nameofnode == "printf")
	{
		for (int i = 0; i < (int)children.size(); i++)
		{
			children[(int)children.size()-i-1]->print(0);
			if(children[(int)children.size()-i-1]->astnode_type == ArrayRefNode || children[(int)children.size()-i-1]->astnode_type == MemberNode)
			{
				cout << tab << "movl (" <<  regstk.back() << "), " << regstk.back() << endl;  	

			}
			cout << tab << "pushl " << regstk.back() << endl; 
		}
		cout << tab << "pushl	$.LC" << printf_counter << "\n    call	printf\n    addl	$"<<4*((int)children.size() + 1)<< ", %esp" << endl;
	}
	else 
	{
		for (const auto& child : children) {
			if (child->data_type.type == 3) {
				for (int j = 0; j < child->data_type.size() / 4; j++) {
					int offset = child->offset + child->data_type.size() - 4 * (j + 1);
					cout << tab << "pushl " << offset << "(%ebp)" << endl;
				}
			} else {
				child->print(0);
				if(child->data_type.array.size() == 0)
				{
					if(child->astnode_type == ArrayRefNode || child->astnode_type == MemberNode){
						cout << "    " << "movl (" <<  regstk.back() << "), " << regstk.back() << endl; // CHANGE	
					}
				}
				cout << tab << "pushl " << regstk.back() << endl;
			}
		}	
		cout << tab << "call	" << procname->nameofnode << "\n    addl	$"<<4*((int)children.size())<< ", %esp" << endl;
	}
}
/////////////////////////////////////

intconst_astnode::intconst_astnode(int val) : exp_astnode()
{
	integer = val;
	astnode_type = IntConstNode;
}

void intconst_astnode::print(int ntabs)
{
	cout << tab << "movl $" << integer << "," << regstk.back() << endl; 
}
/////////////////////////////////////
floatconst_astnode::floatconst_astnode(float val) : exp_astnode()
{
	value = val;
	astnode_type = FloatConstNode;
}

void floatconst_astnode::print(int ntabs)
{
	printAst("", "f", "floatconst", value);
}
///////////////////////////////////
stringconst_astnode::stringconst_astnode(string val) : exp_astnode()
{
	nameofnode = val;
	value = val;
	astnode_type = StringConstNode;
}

void stringconst_astnode::print(int ntabs)
{
	printAst("", "s", "stringconst", stringTocharstar(value));
}

// ref_astnode::ref_astnode() : exp_astnode()
// {
// 	lvalue = true;
// }

/////////////////////////////////

identifier_astnode::identifier_astnode(string val) : ref_astnode()
{
	nameofnode = val;
	astnode_type = IdentifierNode;
	id = val;
}

void identifier_astnode::print(int ntabs)
{
									if (data_type.array.size() && offset <= 0)
									{
										std::string output = "    ";
										output += "leal " + std::to_string(offset) + "(%ebp)," + regstk.back() + "\n";
										std::cout << output;
									}
									else if (data_type.array.size())
									{
										std::string output = "    ";
										output += "movl " + std::to_string(offset) + "(%ebp)," + regstk.back() + "\n";
										std::cout << output;
									}
									else if (rakesh && data_type.type == 3)
									{
										std::string output = "    ";
										output += "leal " + std::to_string(offset) + "(%ebp)," + regstk.back() + "\n";
										rakesh = false;
										std::cout << output;
									}
									else
									{
										std::string output = "movl " + std::to_string(offset) + "(%ebp)," + regstk.back() + "\n";
										std::cout << output;
									}

}

////////////////////////////////

arrayref_astnode::arrayref_astnode(exp_astnode *l, exp_astnode *r) : ref_astnode() // again, changed from ref to exp
{
	left = l;
	right = r;
	id = "ArrayRef";
	astnode_type = ArrayRefNode;
	node_count = l->node_count;
}

void arrayref_astnode::print(int ntabs)
{
	left->print(0);
	string R = regstk.back();
	regstk.pop_back();
	right->print(0);

	int aathu = left->data_type.size();

	int i = 0;
	while( i < (int)(left->data_type.array.size())){
		aathu /= left->data_type.array[i];
		i++;
	}

	i = 1;
	while( i < (int)(left->data_type.array.size())){
		aathu *= left->data_type.array[i];
		i++;
	}

	cout << "    " << "imul $" <<  aathu << ", " << regstk.back() << endl; 
	
	cout << "    " << "addl " <<  regstk.back() << ", " << R << endl;	


	regstk.push_back(R);
}

///////////////////////////////

// pointer_astnode::pointer_astnode(ref_astnode *c) : ref_astnode()
// {
// 	child = c;
// 	id = "Pointer";
// 	astnode_type = PointerNode;
// }

// void pointer_astnode::print(int ntabs)
// {
// 	printAst("", "a", "pointer", child);
// }

////////////////////////////////

deref_astnode::deref_astnode(ref_astnode *c) : ref_astnode()
{
	child = c;
	id = "Deref";
	astnode_type = DerefNode;
}

void deref_astnode::print(int ntabs)
{
	printAst("", "a", "deref", child);
}

/////////////////////////////////

member_astnode::member_astnode(exp_astnode *l, identifier_astnode *r) //  from ref to exp(1st arg)
{
	left = l;
	right = r;
	astnode_type = MemberNode;
	node_count = l->node_count;
}

void member_astnode::print(int ntabs)
{
									rakesh = true;
									left->print(0);
									cout << "    " << "addl $" <<  offset2 << ", " << regstk.back() << endl;
}

/////////////////////////////////

arrow_astnode::arrow_astnode(exp_astnode *l, identifier_astnode *r)
{
	left = l;
	right = r;
	astnode_type = ArrowNode;
	node_count = l->node_count;
}

void arrow_astnode::print(int ntabs)
{

	cout << tab << "movl " <<  offset << "(%ebp)," << regstk.back() << endl;
	cout << tab << "movl " <<  offset2 << "(" << regstk.back() << "), " << regstk.back() << endl;
}
void printblanks(int blanks)
{
	for (int i = 0; i < blanks; i++)
		cout << " ";
}

/////////////////////////////////

void printAst(const char *astname, const char *fmt...) // fmt is a format string that tells about the type of the arguments.
{
	typedef vector<abstract_astnode *> *pv;
	va_list args;
	va_start(args, fmt);
	if ((astname != NULL) && (astname[0] != '\0'))
	{

		;
	}
	while (*fmt != '\0')
	{
		if (*fmt == 'a')
		{
			char *field = va_arg(args, char *);
			abstract_astnode *a = va_arg(args, abstract_astnode *);

			a->print(0);
		}
		else if (*fmt == 's')
		{
			char *field = va_arg(args, char *);
			char *str = va_arg(args, char *);
		}
		else if (*fmt == 'i')
		{
			char *field = va_arg(args, char *);
			int i = va_arg(args, int);
		}
		else if (*fmt == 'f')
		{
			char *field = va_arg(args, char *);
			double f = va_arg(args, double);
		}
		else if (*fmt == 'l')
		{
			char *field = va_arg(args, char *);
			pv f = va_arg(args, pv);
			for (int i = 0; i < (int)f->size(); ++i)
			{
				(*f)[i]->print(0);
				if (i < (int)f->size() - 1)
					;
				else
					;
			}
		}
		++fmt;
		if (*fmt != '\0')
			;
	}
	if ((astname != NULL) && (astname[0] != '\0'))
		va_end(args);
}

char *stringTocharstar(string str)
{
	char *charstar = const_cast<char *>(str.c_str());
	return charstar;
}
