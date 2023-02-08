#pragma once
//----------------------------------Лексический анализатор----------------------------------
//Типы символов
enum signals {Alpha, Digits, Comma, Bl, Br, Star, Plus, Colon, Wrong, Space };

//Состояния автомата
enum lexStates { lS, lA, lB, lC, lD, lE, lF, lG, lH, lI, lJ };

//Матрица автомата
lexStates lexTable[10][11] = {
	//  S   A   B   C   D   E   F   G   H   I   J  
	  {lA, lA, lA, lJ, lS, lJ, lJ, lJ, lJ, lS, lJ}, //A..Z
	  {lB, lA, lB, lD, lS, lF, lJ, lH, lJ, lS, lJ}, //0..9
	  {lC, lS, lS, lJ, lS, lJ, lJ, lJ, lJ, lS, lS}, //,
	  {lE, lS, lS, lJ, lS, lJ, lJ, lJ, lJ, lS, lS}, //(
	  {lJ, lJ, lJ, lJ, lJ, lJ, lJ, lJ, lI, lJ, lJ}, //)
	  {lF, lS, lS, lJ, lS, lJ, lJ, lJ, lJ, lS, lS}, //*
	  {lG, lS, lS, lJ, lS, lJ, lJ, lJ, lJ, lS, lS}, //+
	  {lJ, lJ, lJ, lJ, lJ, lJ, lG, lJ, lJ, lJ, lJ}, //:
	  {lJ, lJ, lJ, lJ, lJ, lJ, lJ, lJ, lJ, lJ, lJ}, //wl 
	  {lS, lS, lS, lJ, lS, lS, lJ, lS, lS, lS, lS}, //
};


//----------------------------------Синтаксический анализатор----------------------------------
//Типы лексем и команд
enum lexType { lSpec,  lMod, lOp, lMark, lAdrrs, lStar, lPlus, lWl, lEnd };

//Типы лексем и команд в строковом виде
const char* lexTypeStr[107] = { "spec", "mod", "op", "mark", "addrs",
	"star", "plus", "wl", "end" };

const char* opTypeStr[64][6] = { {"nop"}, {"add"}, {"sub"}, {"mul"}, {"div"}, {"num", "char", "hlt"}, 
	{"sla", "sra", "slax", "srax", "slc", "src"},{"move"},
	{"lda"}, {"ld1"}, {"ld2"}, {"ld3"}, {"ld4"}, {"ld5"}, {"ld6"}, {"ldx"}, 
	{"ldan"}, {"ld1n"}, {"ld2n"}, {"ld3n"}, {"ld4n"}, {"ld5n"}, {"ld6n"}, {"ldxn"},
	{"sta"}, {"st1"}, {"st2"}, {"st3"}, {"st4"}, {"st5"}, {"st6"}, {"stx"}, 
	{"stj"}, {"stz"}, {"jbus"}, {"ioc"}, {"in"}, {"out"},
	{"jred"}, {"jmp", "jsj", "jov", "jnov"}, {"ja"}, {"j1"}, {"j2"}, {"j3"}, {"j4"}, {"j5"}, {"j6"}, {"jx"}, 
	{"inca", "deca", "enta", "enna"}, {"inc1", "dec1", "ent1", "enn1"}, 
	{"inc2", "dec2", "ent2", "enn2"}, {"inc3", "dec3", "ent3", "enn3"}, 
	{"inc4", "dec4", "ent4", "enn4"}, {"inc5", "dec5", "ent5", "enn5"}, 
	{"inc6", "dec6", "ent6", "enn6"}, {"incx", "decx", "entx", "ennx"},
	{"cmpa"}, {"cmp1"}, {"cmp2"}, {"cmp3"}, {"cmp4"}, {"cmp5"}, {"cmp6"}, {"cmpx" }};


//const char* lexTypeStr[9] = {"spec", "mod", "op", "mark", "num", 
//						"star", "plus", "wl", "end"};

//Типы команд
//enum opType {NOP, ADD, SUB, MUL, DIV, NUM, CHAR, HLT, SLA, SRA, SLAX, SRAX, SLC, SRC, MOVE,
//			LDA, LD1, LD2, LD3, LD4, LD5, LD6, LDX, LDAN, LD1N, LD2N, LD3N, LD4N, LD5N, LD6N, LDXN,
//			STA, ST1, ST2, ST3, ST4, ST5, ST6, STX, STJ, STZ, JBUS, IOC, IN, OUT,
//			JRED, JMP, JSJ, JOV, JNOV, JA, J1, J2, J3, J4, J5, J6, JX, INCA, DECA, ENTA, ENNA,
//			INC1, DEC1, ENT1, ENN1, INC2, DEC2, ENT2, ENN2, INC3, DEC3, ENT3, ENN3, INC4, DEC4,
//			ENT4, ENN4, INC5, DEC5, ENT5, ENN5, INC6, DEC6, ENT6, ENN6, INCX, DECX, ENTX, ENNX, 
//			CMPA, CMP1, CMP2, CMP3, CMP4, CMP5, CMP6, CMPX };

//Состояния автомата
enum syntStates { sS, sA, sB, sC, sD, Er };

//Матрица автомата
syntStates syntTable[9][7] = {
  // S   A   B   C   D	 E	 Er
	{Er, Er, Er, sD, Er, Er}, //Spec
	{Er, Er, Er, sS, sS, Er}, //Mod
	{sB, sB, Er, sS, sS, Er}, //Op
	{sA, Er, sC, Er, Er, Er}, //Mark 
	{Er, Er, sC, Er, Er, Er}, //Adrrs
	{Er, Er, Er, Er, Er, Er}, //Star
	{Er, Er, Er, Er, Er, Er}, //Plus
	{Er, Er, Er, Er, Er, Er}, //Wl
	{sS, Er, Er, sS, sS, Er}, //End
};
