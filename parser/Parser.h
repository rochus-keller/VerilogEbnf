

// This file was automatically generated by Coco/R; don't modify it.
#if !defined(Coco_COCO_PARSER_H__)
#define Coco_COCO_PARSER_H__

#include <QStack>
#include <Verilog/VlSynTree.h>


#include "VlPpLexer.h"

namespace Coco {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
    static void SynErr(const QString& sourcePath, int line, int col, int n, Vl::Errors* err, const char* ctx, const QString& = QString() );
	void Error(int line, int col, const char *s);
	void Warning(int line, int col, const char *s);
	void Warning(const char *s);
	void Exception(const char *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_TPlus=1,
		_TMinus=2,
		_TBang=3,
		_TBangEq=4,
		_TBang2Eq=5,
		_TTilde=6,
		_TTildeBar=7,
		_TTildeAmp=8,
		_TTildeHat=9,
		_THat=10,
		_THatTilde=11,
		_TSlash=12,
		_TPercent=13,
		_TEq=14,
		_T2Eq=15,
		_T3Eq=16,
		_TAmp=17,
		_T2Amp=18,
		_TBar=19,
		_T2Bar=20,
		_TStar=21,
		_T2Star=22,
		_TLt=23,
		_TLeq=24,
		_T2Lt=25,
		_T3Lt=26,
		_TGt=27,
		_TGeq=28,
		_T2Gt=29,
		_T3Gt=30,
		_THash=31,
		_TAt=32,
		_TQmark=33,
		_TEqGt=34,
		_TStarGt=35,
		_TLpar=36,
		_TRpar=37,
		_TLbrack=38,
		_TRbrack=39,
		_TLbrace=40,
		_TRbrace=41,
		_TLatt=42,
		_TRatt=43,
		_TLcmt=44,
		_TRcmt=45,
		_TComma=46,
		_TDot=47,
		_TSemi=48,
		_TColon=49,
		_TPlusColon=50,
		_TMinusColon=51,
		_Talways=52,
		_Tand=53,
		_Tassign=54,
		_Tautomatic=55,
		_Tbegin=56,
		_Tbuf=57,
		_Tbufif0=58,
		_Tbufif1=59,
		_Tcase=60,
		_Tcasex=61,
		_Tcasez=62,
		_Tcell=63,
		_Tcmos=64,
		_Tconfig=65,
		_Tdeassign=66,
		_Tdefault=67,
		_Tdefparam=68,
		_Tdesign=69,
		_Tdisable=70,
		_Tedge=71,
		_Telse=72,
		_Tend=73,
		_Tendcase=74,
		_Tendconfig=75,
		_Tendfunction=76,
		_Tendgenerate=77,
		_Tendmodule=78,
		_Tendprimitive=79,
		_Tendspecify=80,
		_Tendtable=81,
		_Tendtask=82,
		_Tevent=83,
		_Tfor=84,
		_Tforce=85,
		_Tforever=86,
		_Tfork=87,
		_Tfunction=88,
		_Tgenerate=89,
		_Tgenvar=90,
		_Thighz0=91,
		_Thighz1=92,
		_Tif=93,
		_Tifnone=94,
		_Tincdir=95,
		_Tinclude=96,
		_Tinitial=97,
		_Tinout=98,
		_Tinput=99,
		_Tinstance=100,
		_Tinteger=101,
		_Tjoin=102,
		_Tlarge=103,
		_Tliblist=104,
		_Tlibrary=105,
		_Tlocalparam=106,
		_Tmacromodule=107,
		_Tmedium=108,
		_Tmodule=109,
		_Tnand=110,
		_Tnegedge=111,
		_Tnmos=112,
		_Tnor=113,
		_Tnoshowcancelled=114,
		_Tnot=115,
		_Tnotif0=116,
		_Tnotif1=117,
		_Tor=118,
		_Toutput=119,
		_Tparameter=120,
		_Tpmos=121,
		_Tposedge=122,
		_Tprimitive=123,
		_Tpull0=124,
		_Tpull1=125,
		_Tpulldown=126,
		_Tpullup=127,
		_Tpulsestyle_onevent=128,
		_Tpulsestyle_ondetect=129,
		_Trcmos=130,
		_Treal=131,
		_Trealtime=132,
		_Treg=133,
		_Trelease=134,
		_Trepeat=135,
		_Trnmos=136,
		_Trpmos=137,
		_Trtran=138,
		_Trtranif0=139,
		_Trtranif1=140,
		_Tscalared=141,
		_Tshowcancelled=142,
		_Tsigned=143,
		_Tsmall=144,
		_Tspecify=145,
		_Tspecparam=146,
		_Tstrong0=147,
		_Tstrong1=148,
		_Tsupply0=149,
		_Tsupply1=150,
		_Ttable=151,
		_Ttask=152,
		_Ttime=153,
		_Ttran=154,
		_Ttranif0=155,
		_Ttranif1=156,
		_Ttri=157,
		_Ttri0=158,
		_Ttri1=159,
		_Ttriand=160,
		_Ttrior=161,
		_Ttrireg=162,
		_Tunsigned=163,
		_Tuse=164,
		_Tuwire=165,
		_Tvectored=166,
		_Twait=167,
		_Twand=168,
		_Tweak0=169,
		_Tweak1=170,
		_Twhile=171,
		_Twire=172,
		_Twor=173,
		_Txnor=174,
		_Txor=175,
		_TPathPulse=176,
		_TSetup=177,
		_THold=178,
		_TSetupHold=179,
		_TRecovery=180,
		_TRemoval=181,
		_TRecrem=182,
		_TSkew=183,
		_TTimeSkew=184,
		_TFullSkew=185,
		_TPeriod=186,
		_TWidth=187,
		_TNoChange=188,
		_TString=189,
		_TIdent=190,
		_TSysName=191,
		_TCoDi=192,
		_TRealnum=193,
		_TNatural=194,
		_TSizedBased=195,
		_TBasedInt=196,
		_TBaseFormat=197,
		_TBaseValue=198,
		_TAttribute=199
	};
	int maxT;

	int errDist;
	int minErrDist;

	void SynErr(int n, const char* ctx = 0);
	void Get();
	void Expect(int n, const char* ctx = 0);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Vl::PpLexer *scanner;
	Vl::Errors  *errors;

	Vl::Token d_cur;
	Vl::Token d_next;
	struct TokDummy
	{
		int kind;
	};
	TokDummy d_dummy;
	TokDummy *la;			// lookahead token
	
	int peek( quint8 la = 1 );

Vl::SynTree d_root;
	QStack<Vl::SynTree*> d_stack;
	void addTerminal() {
		Vl::SynTree* n = new Vl::SynTree( d_cur ); d_stack.top()->d_children.append(n);
	}



	Parser(Vl::PpLexer *scanner,Vl::Errors*);
	~Parser();
	void SemErr(const char* msg);

	void Verilog05();
	void translation_unit();
	void identifier();
	void system_name();
	void real_number();
	void natural_number();
	void sizedbased_number();
	void based_number();
	void base_format();
	void base_value();
	void string();
	void unary_operator();
	void binary_operator();
	void unary_module_path_operator();
	void binary_module_path_operator();
	void module_declaration();
	void udp_declaration();
	void config_declaration();
	void library_declaration();
	void include_statement();
	void library_identifier();
	void file_path_spec();
	void module_keyword();
	void module_identifier();
	void module_parameter_port_list();
	void list_of_ports();
	void list_of_port_declarations();
	void module_item();
	void parameter_declaration();
	void port();
	void port_declaration();
	void port_expression();
	void port_identifier();
	void port_reference();
	void constant_range_expression();
	void inout_declaration();
	void input_declaration();
	void output_declaration();
	void non_port_module_item();
	void module_or_generate_item();
	void module_or_generate_item_declaration();
	void local_parameter_declaration();
	void parameter_override();
	void continuous_assign();
	void gate_instantiation();
	void module_or_udp_instantiation();
	void initial_construct();
	void always_construct();
	void loop_generate_construct();
	void conditional_generate_construct();
	void net_declaration();
	void reg_declaration();
	void integer_declaration();
	void real_declaration();
	void time_declaration();
	void realtime_declaration();
	void event_declaration();
	void genvar_declaration();
	void task_declaration();
	void function_declaration();
	void generate_region();
	void specify_block();
	void specparam_declaration();
	void list_of_defparam_assignments();
	void config_identifier();
	void design_statement();
	void config_rule_statement();
	void cell_identifier();
	void default_clause();
	void inst_clause();
	void cell_clause();
	void liblist_clause();
	void use_clause();
	void inst_name();
	void topmodule_identifier();
	void instance_identifier();
	void range();
	void parameter_type();
	void list_of_param_assignments();
	void list_of_specparam_assignments();
	void net_type();
	void list_of_port_identifiers();
	void list_of_variable_port_identifiers();
	void output_variable_type();
	void list_of_event_identifiers();
	void list_of_variable_identifiers();
	void list_of_net_decl_assignments_or_identifiers();
	void net_identifier();
	void dimension();
	void expression();
	void drive_strength();
	void charge_strength();
	void delay3();
	void list_of_real_identifiers();
	void real_type();
	void real_identifier();
	void constant_expression();
	void variable_type();
	void variable_identifier();
	void strength0();
	void strength1();
	void delay();
	void delay_value();
	void mintypmax_expression();
	void expression_nlr();
	void delay2();
	void unsigned_or_real_number();
	void defparam_assignment();
	void event_identifier();
	void param_assignment();
	void specparam_assignment();
	void hierarchical_parameter_identifier();
	void constant_mintypmax_expression();
	void parameter_identifier();
	void specparam_identifier();
	void pulse_control_specparam();
	void reject_limit_value();
	void error_limit_value();
	void limit_value();
	void dimension_constant_expression();
	void msb_constant_expression();
	void lsb_constant_expression();
	void function_range_or_type();
	void function_identifier();
	void function_port_list();
	void function_item_declaration();
	void function_statement();
	void block_item_declaration();
	void tf_input_declaration();
	void task_identifier();
	void task_port_list();
	void task_item_declaration();
	void statement_or_null();
	void tf_output_declaration();
	void tf_inout_declaration();
	void task_port_item();
	void task_port_type();
	void block_reg_declaration();
	void block_integer_declaration();
	void block_time_declaration();
	void block_real_declaration();
	void block_realtime_declaration();
	void list_of_block_variable_identifiers();
	void list_of_block_real_identifiers();
	void block_variable_type();
	void block_real_type();
	void cmos_switchtype();
	void cmos_switch_instance();
	void enable_gatetype();
	void enable_gate_instance();
	void mos_switchtype();
	void mos_switch_instance();
	void n_input_gatetype();
	void n_input_gate_instance();
	void n_output_gatetype();
	void n_output_gate_instance();
	void pass_en_switchtype();
	void pass_enable_switch_instance();
	void pass_switchtype();
	void pass_switch_instance();
	void pulldown_strength();
	void pull_gate_instance();
	void pullup_strength();
	void name_of_gate_instance();
	void output_terminal();
	void input_terminal();
	void ncontrol_terminal();
	void pcontrol_terminal();
	void enable_terminal();
	void input_or_output_terminal();
	void expression_2();
	void inout_terminal();
	void gate_instance_identifier();
	void net_lvalue();
	void named_parameter_assignment();
	void list_of_genvar_identifiers();
	void genvar_identifier();
	void genvar_initialization();
	void genvar_expression();
	void genvar_iteration();
	void generate_block();
	void genvar_primary();
	void genvar_expression_nlr();
	void constant_primary();
	void if_generate_construct();
	void case_generate_construct();
	void generate_block_or_null();
	void case_generate_item();
	void generate_block_identifier();
	void udp_identifier();
	void udp_port_list();
	void udp_declaration_port_list();
	void udp_port_declaration();
	void udp_body();
	void output_port_identifier();
	void input_port_identifier();
	void udp_output_declaration();
	void udp_input_declaration();
	void udp_reg_declaration();
	void udp_initial_statement();
	void sequential_or_combinatorial_entry();
	void init_val();
	void level_or_edge_symbol();
	void level_symbol();
	void number();
	void edge_descriptor();
	void scalar_constant();
	void list_of_net_assignments();
	void net_assignment();
	void statement();
	void procedural_continuous_assignments();
	void variable_assignment();
	void variable_lvalue();
	void variable_or_net_lvalue();
	void par_block();
	void block_identifier();
	void seq_block();
	void blocking_or_nonblocking_assignment_or_task_enable();
	void delay_or_event_control();
	void case_statement();
	void conditional_statement();
	void disable_statement();
	void event_trigger();
	void loop_statement();
	void procedural_timing_control_statement();
	void system_task_enable();
	void wait_statement();
	void delay_control();
	void event_control();
	void hierarchical_task_or_block_identifier();
	void hierarchical_identifier();
	void event_expression();
	void hierarchical_event_identifier();
	void hierarchical_identifier_range();
	void event_expression_nlr();
	void procedural_timing_control();
	void case_item();
	void system_task_identifier();
	void specify_item();
	void pulsestyle_declaration();
	void showcancelled_declaration();
	void path_declaration();
	void system_timing_check();
	void list_of_path_outputs();
	void simple_or_edge_sensitive_path_declaration();
	void state_dependent_path_declaration();
	void parallel_or_full_path_description();
	void list_of_path_inputs();
	void polarity_operator();
	void specify_output_terminal_descriptor();
	void simple_path_declaration();
	void path_delay_value();
	void specify_input_terminal_descriptor();
	void input_identifier();
	void output_identifier();
	void input_or_inout_port_identifier();
	void output_or_inout_port_identifier();
	void list_of_path_delay_expressions();
	void path_delay_expression();
	void edge_identifier();
	void data_source_expression();
	void simple_or_edge_sensitive_path_description();
	void module_path_expression();
	void dlr_setup_timing_check();
	void dlr_hold_timing_check();
	void dlr_setuphold_timing_check();
	void dlr_recovery_timing_check();
	void dlr_removal_timing_check();
	void dlr_recrem_timing_check();
	void dlr_skew_timing_check();
	void dlr_timeskew_timing_check();
	void dlr_fullskew_timing_check();
	void dlr_period_timing_check();
	void dlr_width_timing_check();
	void dlr_nochange_timing_check();
	void data_event();
	void reference_event();
	void timing_check_limit();
	void notifier();
	void stamptime_condition();
	void checktime_condition();
	void delayed_reference();
	void delayed_data();
	void event_based_flag();
	void remain_active_flag();
	void controlled_reference_event();
	void threshold();
	void start_edge_offset();
	void end_edge_offset();
	void controlled_timing_check_event();
	void timing_check_event();
	void terminal_identifier();
	void timing_check_event_control();
	void specify_terminal_descriptor();
	void timing_check_condition();
	void edge_control_specifier();
	void specify_input_or_output_terminal_descriptor();
	void scalar_timing_check_condition();
	void constant_expression_nlr();
	void width_constant_expression();
	void primary();
	void module_path_primary();
	void module_path_expression_nlr();
	void module_path_mintypmax_expression();
	void range_expression();
	void system_function_identifier();
	void hierarchical_identifier_range_const();
	void parameter_value_assignment_or_delay2();
	void module_or_udp_instance();
	void port_connection_or_output_terminal();
	void primary_2();

	void Parse();

}; // end Parser

} // namespace


#endif
