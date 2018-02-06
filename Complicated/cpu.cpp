#include "systemc.h"
//------------------------------------------------------------------------------------------------pc
SC_MODULE (PC)
{
	sc_in<sc_lv<4>> pcin;
	sc_in<bool> pcwrite,clk;
	sc_out<sc_lv<4>> pcout;
	
	SC_CTOR (PC)
	{
		SC_METHOD (process);
		sensitive<<clk.pos();
	}  
	void process()
	{
		if(pcwrite.read() == '1') 
		{	
			pcout.write(pcin.read());
		}
    }
};   
//------------------------------------------------------------------------------------------------adderpc

SC_MODULE (adderpc)
{
	sc_in<sc_lv<4>> din;
	sc_out<sc_lv<4>> dout;

	SC_CTOR(adderpc)
	{
		SC_METHOD (process);
		sensitive << din;
	}
	void process()
	{

		sc_uint<4> temp1 = din.read() ;
		sc_uint<4> temp2 = 1;
		sc_lv<4> temp3 ;
		temp3=temp1+temp2;
		dout.write(temp3);
	}
};
//------------------------------------------------------------------------------------------------instmem

SC_MODULE (instmem)
{
	sc_in<sc_lv<4>> address;
	sc_in<bool> enout;
	sc_out<sc_lv<4>> opcode;
	sc_out<sc_lv<13>> R1,R2;
	sc_lv <30> memory[16];
	SC_CTOR(instmem)
	{
		SC_METHOD (process);
		sensitive << address;
	}
	void process()
	{
		memory[0]="000000000000000000000000000000";
		memory[1]="000100000000000000000000000000";
		memory[2]="011000000000000000000000000000";
		memory[3]="001100000000000000000000000000";
		memory[4]="010000000000000000000000000000";
		memory[5]="100100000000000000000000000000";
		memory[6]="000000000000000000000000000000";
		memory[7]="001000000000000000000000000000";
		memory[8]="100000000000000000000000000000";
		memory[9]="000000000000000000000000000000";
		memory[10]="001100000000000000000000000000";
		memory[11]="000000000000000000000000000000";
		memory[12]="011000000000000000000000000000";
		memory[13]="010000000000000000000000000000";
		memory[14]="000100000000000000000000000000";
		memory[15]="000000000000000000000000000000";
		sc_uint<4> temp=address.read();
		sc_uint<30> tempinst;
		sc_uint<4> temp1;
		sc_uint<13> temp2,temp3;
		tempinst=memory[temp];
		temp1=tempinst.range(3,0);
		temp2=tempinst.range(16,4);
		temp3=tempinst.range(29,17);
		if(enout.read() == '1')
		{
			opcode.write(temp1);
			R1.write(temp2);
			R2.write(temp3);
		}
	}
};
//------------------------------------------------------------------------------------------------alu

SC_MODULE (alu)
{
	sc_in<sc_lv<8>> a,b;
	sc_in<sc_lv<4>> sel;
	sc_out<sc_lv<8>> f;
	
	SC_CTOR (alu)
	{
		SC_METHOD (process);
		sensitive << a << b << sel ;
	}

	void process()
	{
		sc_lv<8> temp1,temp2;
		sc_uint<64> temp3,temp4;
		temp3=a.read();
		temp4=b.read();
		if(sel.read() == "0000")
			f.write(a.read());
		else if(sel.read() == "0001")
			f.write(temp3 + 1);
		else if(sel.read() == "0010")
			f.write(temp3 + temp4);
		else if(sel.read() == "0011")
			f.write(temp3 - 1);
		else if(sel.read() == "0100")
			f.write(temp3 - temp4);
		else if(sel.read() == "0101")
			f.write(a.read() & b.read());
		else if(sel.read() == "0110")
			f.write(a.read() | b.read());
		else if(sel.read() == "0111")
			f.write(a.read() ^ b.read());
		else if(sel.read() == "1000")
		{
			temp1=a.read();
			temp2[31]='0';
			temp2.range(30,0)=temp1.range(31,1);
			f.write(temp2);
		}
		else if(sel.read() == "1001")
		{	temp1=a.read();
			temp2[0]='0';
			temp2.range(31,1)=temp1.range(30,0);
			f.write(temp2);
		}
	}
};
//------------------------------------------------------------------------------------------------datamem

SC_MODULE (datamem)
{
	sc_in<sc_lv<13>> address;
	sc_in<sc_lv<8>> data;
	sc_in<bool> read,write;
	sc_out<sc_lv<8>> readdata;
	sc_lv <8> memory[8192];
	SC_CTOR(datamem)
	{
		SC_METHOD (process);
		sensitive << address;
	}
	void process()
	{
		sc_uint<13> temp=address.read();
		if(read.read() == '1')
			readdata.write(memory[temp]);
		else if(write.read() == '1')
			memory[temp]=data.read();
	}
};
//------------------------------------------------------------------------------------------------registerbAB

SC_MODULE(registerAB)
{
	sc_in<sc_lv<13>> address;
	sc_in<sc_lv<8>> data;
	sc_in<bool> regwrite,clk;
	sc_lv <8> reg[8192];
	SC_CTOR(registerAB)
	{
		SC_METHOD (process);
		sensitive << clk;
	}
	void process()
	{
		sc_uint<13> temp=address.read();
		if(regwrite.read() == '1')
		{
			reg[temp]=data.read();
		}
	}
};
//------------------------------------------------------------------------------------------------mux2_1_8bit

SC_MODULE(mux2_1_8bit)
{
	sc_in<sc_lv<8>> a,b;
	sc_in<bool> sel;
	sc_out<sc_lv<8>> output;
	SC_CTOR(mux2_1_8bit)
	{
		SC_METHOD (process);
		sensitive << sel;
	}
	void process()
	{
		if(sel.read() == '1')
			output.write(a.read());
		else
			output.write(b.read());
	}
};
//------------------------------------------------------------------------------------------------controller

SC_MODULE(controller)
{
	sc_in<bool> clk,clr;
	sc_in<sc_lv<4>> opcode;
	sc_out<bool> pcwrite,memwrite,memread,regwrite,eninstmem,selmux;
	sc_out<sc_lv<4>> aluop;
	enum state {s0,s1,s2,s3,s4,s5,s6};
	sc_signal<state> next_state,pr_state;
	
	SC_CTOR(controller)
	{
		SC_METHOD (process1);
		sensitive << clk.pos()<<clr;
		SC_METHOD (process2);
		sensitive << opcode<<pr_state;
	}
	void process1()
	{
		if(clr.read() == '1')
			pr_state=s0;
		else
			pr_state<=next_state;
	}
	void process2()
	{
		switch(pr_state)
		{
			case s0: //fetch
				pcwrite.write('1');
				memread.write('0');
				memwrite.write('0');
				regwrite.write('0');
				eninstmem.write('0');
				aluop.write("0000");
				next_state=s1;

			case s1: //decode
				pcwrite.write('0');
				memread.write('0');
				memwrite.write('0');
				regwrite.write('0');
				eninstmem.write('1');
				aluop.write("0000");
				next_state=s2;

			case s2:  //execute
				if(opcode.read() == "0000") 
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0000");
					next_state=s3;
				}
				else if(opcode.read() == "0001") //inc
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0001");
					next_state=s3;
				}
				else if(opcode.read() == "0010") //add
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0010");
					next_state=s3;
				}
				else if(opcode.read() == "0011") //dec
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0011");
					next_state=s3;
				}
				else if(opcode.read() == "0100") //sub
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0100");
					next_state=s3;
				}
				else if(opcode.read() == "0101") //and
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0101");
					next_state=s3;
				}
				else if(opcode.read() == "0110") //or
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0110");
					next_state=s3;
				}
				else if(opcode.read() == "0111") //xor
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("0111");
					next_state=s3;
				}
				else if(opcode.read() == "1000") //sr
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("1000");
					next_state=s3;
				}
				else if(opcode.read() == "1001") //lr
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					aluop.write("1001");
					next_state=s3;
				}
				else if(opcode.read() == "1010") //sw
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					next_state=s4;
				}
				else if(opcode.read() == "1011") //lw
				{
					pcwrite.write('0');
					memread.write('0');
					memwrite.write('0');
					regwrite.write('0');
					eninstmem.write('0');
					next_state=s5;
				}

			case s3:
				selmux.write('1');
				pcwrite.write('0');
				memread.write('0');
				memwrite.write('1');
				regwrite.write('0');
				eninstmem.write('0');
				next_state=s0;

			case s4:
				selmux.write('0');
				pcwrite.write('0');
				memread.write('0');
				memwrite.write('1');
				regwrite.write('0');
				eninstmem.write('0');
				next_state=s0;

			case s5:
				pcwrite.write('0');
				memread.write('1');
				memwrite.write('0');
				regwrite.write('0');
				eninstmem.write('0');
				next_state=s6;

			case s6:
				pcwrite.write('0');
				memread.write('0');
				memwrite.write('0');
				regwrite.write('1');
				eninstmem.write('0');
				next_state=s0;
		}
	}
};
//------------------------------------------------------------------------------------------------datapath

SC_MODULE(datapath)
{
	sc_in<bool> clk,pcwrite,eninstmem,memwrite,memread,regwrite,selmux;
	sc_in<sc_lv<8>> A,B;
	sc_in<sc_lv<4>> aluop;
	sc_out<sc_lv<4>> opcode;
	sc_out<sc_lv<8>> F;

	PC pc1;
	adderpc adder1;
	instmem instmem1;
	alu alu1;
	mux2_1_8bit mux1;
	datamem datamem1;
	registerAB registerAB1;

	sc_signal<sc_lv<4>> pcin,pcout;
	sc_signal<sc_lv<13>> R1,R2;
	sc_signal<sc_lv<8>> outmux,outmem;

	SC_CTOR(datapath):pc1("pc1"),adder1("adder1"),alu1("alu1"),instmem1("instmem1"),datamem1("datamem1"),registerAB1("registerAB1"),mux1("mux1")
	{
		pc1.pcin(pcin);
		pc1.pcwrite(pcwrite);
		pc1.clk(clk);
		pc1.pcout(pcout);
		adder1<<pcout<<pcin;
		instmem1<<pcout<<eninstmem<<opcode<<R1<<R2;
		alu1<<A<<B<<aluop<<F;
		mux1<<A<<F<<selmux<<outmux;
		datamem1<<R1<<outmux<<memread<<memwrite<<outmem;
		registerAB1<<R1<<outmem<<regwrite<<clk;
	}
};
//------------------------------------------------------------------------------------------------controller


int sc_main(int argc, char* argv[])
{
	sc_signal<bool> clk,clr,pcwrite,eninstmem,memwrite,memread,regwrite,selmux;
	sc_signal<sc_lv<8>> A,B;
	sc_signal<sc_lv<4>> aluop;
	sc_signal<sc_lv<4>> opcode;
	sc_signal<sc_lv<8>> F;
	int i = 0;
	
	controller controller1("controller1");
	controller1<<clk<<clr<<opcode<<pcwrite<<memwrite<<memread<<regwrite<<eninstmem<<selmux<<aluop;

	datapath datapath1("datapath1");
	datapath1<<clk<<pcwrite<<eninstmem<<memwrite<<memread<<regwrite<<selmux<<A<<B<<aluop<<opcode<<F;

	sc_start(1,SC_NS);
	sc_trace_file *wf = sc_create_vcd_trace_file("cpu");
	sc_trace(wf, clk, "clk");
    sc_trace(wf, clr, "clr");
    sc_trace(wf, pcwrite, "pcwrite");
    sc_trace(wf, eninstmem, "eninstmem");
	sc_trace(wf, memwrite, "memwrite");
    sc_trace(wf, memread, "memread");
    sc_trace(wf, regwrite, "regwrite");
    sc_trace(wf, selmux, "selmux");
	sc_trace(wf, A, "A");
    sc_trace(wf, B, "B");
    sc_trace(wf, aluop, "aluop");
	sc_trace(wf, opcode, "opcode");
    sc_trace(wf, F, "F");

	A = 12;       
    B = 14;      
    for (i=0;i<5;i++) 
	{
		clk = 0; 
		sc_start(1,SC_NS);
		clk = 1; 
		sc_start(1,SC_NS);
    }
	A=0;
	cout << "@" << sc_time_stamp() <<"       A=0             \n" << endl;
	for (i=0;i<5;i++) 
	{
		clk = 0; 
		sc_start(1,SC_NS);
		clk = 1; 
		sc_start(1,SC_NS);
    }
	B=0;
	cout << "@" << sc_time_stamp() <<"       B=0             \n" << endl;
	for (i=0;i<5;i++) 
	{
		clk = 0; 
		sc_start(1,SC_NS);
		clk = 1; 
		sc_start(1,SC_NS);
    }

	sc_close_vcd_trace_file(wf);
	return(0);
}

