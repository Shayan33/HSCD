
#include <systemc.h>

        
SC_MODULE(ALU){
   sc_in< sc_uint<8> > A,B;
   sc_in< sc_lv<4> > S;
   sc_out< sc_uint<8> > F;
   void func(){
   if (S.read() == "0000")
   {
     F.write(A.read());
    
      cout<<"@" << sc_time_stamp() << " :: Executed func is 0 " <<endl;
   }
   else if (S.read() == "0001")
   {
     F.write(A.read()+1);
     cout<<"@" << sc_time_stamp() << " :: Executed func is 1 " <<endl;
   }
   else if (S.read() == "0010")
   {
     F.write(A.read()+B.read());
     cout<<"@" << sc_time_stamp() << " :: Executed func is 2 " <<endl;
   }
   else if (S.read() == "0011")
   {
     F.write(A.read()-1);
      cout<<"@" << sc_time_stamp() << " :: Executed func is 3 " <<endl;
   }
   else if (S.read() == "0100")
   {
     F.write(A.read() - B.read());
      cout<<"@" << sc_time_stamp() << " :: Executed func is 4 " <<endl;
   }
   else if (S.read() == "0101")
   {
     F.write(A.read() & B.read());
      cout<<"@" << sc_time_stamp() << " :: Executed func is 5 " <<endl;
   }
   else if (S.read() == "0110")
   {
     F.write(A.read() | B.read());
      cout<<"@" << sc_time_stamp() << " :: Executed func is 6 " <<endl;
   }
   else if (S.read() == "0111")
   {
     F.write(A.read() ^ B.read());
      cout<<"@" << sc_time_stamp() << " :: Executed func is 7 " <<endl;
   }
   else if (S.read() == "1000")
   {
     F.write(A.read() / 2);
      cout<<"@" << sc_time_stamp() << " :: Executed func is 8 " <<endl;
   }
   else if (S.read() == "1001")
   {
     F.write(A.read() * 2);
      cout<<"@" << sc_time_stamp() << " :: Executed func is 9 " <<endl;
   }
   else 
     cout<<"invalid operation"<<endl;
     }
SC_CTOR(ALU){
   cout<<"Call ALU"<<endl;
   SC_METHOD(func);
   sensitive << S;
}
};

SC_MODULE (Memory)
{
	sc_in<bool> enable;
	sc_in<bool> readwrite;
	sc_inout<sc_lv<4> > databus;
	sc_in<sc_lv<4> > addrbus;
	sc_lv<4>mem[16];
  
  void process()
{
    mem[0]  = "0001";
  	mem[1]  = "0011";
  	mem[2]  = "0110";
  	mem[3]  = "0100";
  	mem[4]  = "1000";
  	mem[5]  = "0010";
  	mem[6]  = "0101";
  	mem[7]  = "0111";
  	mem[8]  = "1001";
  	mem[9]  = "1000";
  	mem[10] = "0001";
  	mem[11] = "0000";
  	mem[12] = "0010";
  	mem[13] = "0100";
  	mem[14] = "0111";
  	mem[15] = "0000";
    
	if (enable.read() == 0)
{
	databus.write("zzzz");
  	return;
    }
 else
{
	sc_lv<4> addr_lv = addrbus.read();
	sc_uint<4> addr = addr_lv;
   if (readwrite.read() == 1)
   {
	databus.write(mem[addr]);
   }

	mem[addr] = databus.read();
}
}

 SC_CTOR(Memory)
{
    cout<<"Call Memory"<<endl;
	SC_METHOD(process);
	sensitive << enable;
	sensitive << readwrite;
    sensitive << addrbus;
}
};  

  SC_MODULE (Display)
  {
    sc_in< sc_uint<8> > D;
    
    void show()
    {
      cout<< " Result is : " << D << endl;
    }
    
    SC_CTOR(Display)
    {
      SC_METHOD(show);
      sensitive << D;
    }
};


int sc_main (int argc, char* argv[]) {
  sc_signal<bool>   readwrite;
  sc_signal<bool>   enable;
  sc_signal<sc_lv<4> > databus;
  sc_signal<sc_lv<4> > addrbus;
  
  sc_signal< sc_uint<8> > A,B;
  sc_signal< sc_uint<8> > F;
  
  Memory InsMemory("InsMemory");
  InsMemory.enable(enable);
  InsMemory.readwrite(readwrite);
  InsMemory.databus(databus);
  InsMemory.addrbus(addrbus);
  
  ALU ALU_Unit("ALU_Unit");
  ALU_Unit.A(A);
  ALU_Unit.B(B);
  ALU_Unit.S(databus);
  ALU_Unit.F(F);
  
  Display Display_Unit("Display_Unit");
  Display_Unit.D(F);
  
  sc_start(1, SC_NS);
  
  
  // Testbench
  A = 4;
  B = 3;
  
  sc_start(2, SC_NS);
 
  addrbus = "0000";
  readwrite = 1;
  
  sc_start(2, SC_NS);
 
  enable = 1;
  cout << "@" << sc_time_stamp() <<" Asserting enable\n" << endl;

  sc_start(5, SC_NS);
  
  
  addrbus = "0001";
  
  
  sc_start(5, SC_NS);

  
  addrbus = "0010";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "0011";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "0100";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "0101";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "0110";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "0111";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "1000";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "1001";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "1010";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "1011";
  
  
  sc_start(5, SC_NS);
  
  
  addrbus = "1100";
  
  
  sc_start(5, SC_NS);
  
  
  enable = 0;
  cout << "@" << sc_time_stamp() <<" De-Asserting enable\n" << endl;
  
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;

  return 0; // Terminate simulation
  
}
