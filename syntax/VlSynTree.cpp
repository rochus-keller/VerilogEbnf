// This file was automatically generated by VerilogEbnf; don't modify it!
#include "VlSynTree.h"
using namespace Vl;

SynTree::SynTree(quint16 r, const Token& t ):d_tok(r){
	d_tok.d_lineNr = t.d_lineNr;
	d_tok.d_colNr = t.d_colNr;
	d_tok.d_sourcePath = t.d_sourcePath;
}

const char* SynTree::rToStr( quint16 r ) {
	switch(r) {
		case R_Attribute: return "attribute_instance";
		case R_MacroUsage: return "text_macro_usage";
		case R_always_construct: return "always_construct";
		case R_block_integer_declaration: return "block_integer_declaration";
		case R_block_real_declaration: return "block_real_declaration";
		case R_block_real_type: return "block_real_type";
		case R_block_realtime_declaration: return "block_realtime_declaration";
		case R_block_reg_declaration: return "block_reg_declaration";
		case R_block_time_declaration: return "block_time_declaration";
		case R_block_variable_type: return "block_variable_type";
		case R_blocking_or_nonblocking_assignment_or_task_enable: return "blocking_or_nonblocking_assignment_or_task_enable";
		case R_case_generate_construct: return "case_generate_construct";
		case R_case_generate_item: return "case_generate_item";
		case R_case_item: return "case_item";
		case R_case_statement: return "case_statement";
		case R_cell_clause: return "cell_clause";
		case R_charge_strength: return "charge_strength";
		case R_cmos_switch_instance: return "cmos_switch_instance";
		case R_conditional_statement: return "conditional_statement";
		case R_config_declaration: return "config_declaration";
		case R_config_rule_statement: return "config_rule_statement";
		case R_constant_expression: return "constant_expression";
		case R_constant_mintypmax_expression: return "constant_mintypmax_expression";
		case R_constant_range_expression: return "constant_range_expression";
		case R_continuous_assign: return "continuous_assign";
		case R_controlled_timing_check_event: return "controlled_timing_check_event";
		case R_default_clause: return "default_clause";
		case R_defparam_assignment: return "defparam_assignment";
		case R_delay: return "delay";
		case R_delay_or_event_control: return "delay_or_event_control";
		case R_delayed_data: return "delayed_data";
		case R_delayed_reference: return "delayed_reference";
		case R_design_statement: return "design_statement";
		case R_dimension: return "dimension";
		case R_disable_statement: return "disable_statement";
		case R_dlr_fullskew_timing_check: return "$fullskew_timing_check";
		case R_dlr_hold_timing_check: return "$hold_timing_check";
		case R_dlr_nochange_timing_check: return "$nochange_timing_check";
		case R_dlr_period_timing_check: return "$period_timing_check";
		case R_dlr_recovery_timing_check: return "$recovery_timing_check";
		case R_dlr_recrem_timing_check: return "$recrem_timing_check";
		case R_dlr_removal_timing_check: return "$removal_timing_check";
		case R_dlr_setup_timing_check: return "$setup_timing_check";
		case R_dlr_setuphold_timing_check: return "$setuphold_timing_check";
		case R_dlr_skew_timing_check: return "$skew_timing_check";
		case R_dlr_timeskew_timing_check: return "$timeskew_timing_check";
		case R_dlr_width_timing_check: return "$width_timing_check";
		case R_drive_strength: return "drive_strength";
		case R_edge_control_specifier: return "edge_control_specifier";
		case R_edge_descriptor: return "edge_descriptor";
		case R_enable_gate_instance: return "enable_gate_instance";
		case R_event_control: return "event_control";
		case R_event_declaration: return "event_declaration";
		case R_event_expression: return "event_expression";
		case R_event_trigger: return "event_trigger";
		case R_expression: return "expression";
		case R_expression_2: return "expression_2";
		case R_file_path_spec: return "file_path_spec";
		case R_function_declaration: return "function_declaration";
		case R_function_range_or_type: return "function_range_or_type";
		case R_gate_instantiation: return "gate_instantiation";
		case R_generate_block: return "generate_block";
		case R_generate_region: return "generate_region";
		case R_genvar_declaration: return "genvar_declaration";
		case R_genvar_expression: return "genvar_expression";
		case R_genvar_initialization: return "genvar_initialization";
		case R_genvar_iteration: return "genvar_iteration";
		case R_hierarchical_identifier: return "hierarchical_identifier";
		case R_hierarchical_identifier_range: return "hierarchical_identifier_range";
		case R_hierarchical_identifier_range_const: return "hierarchical_identifier_range_const";
		case R_if_generate_construct: return "if_generate_construct";
		case R_include_statement: return "include_statement";
		case R_initial_construct: return "initial_construct";
		case R_inout_declaration: return "inout_declaration";
		case R_input_declaration: return "input_declaration";
		case R_inst_clause: return "inst_clause";
		case R_inst_name: return "inst_name";
		case R_integer_declaration: return "integer_declaration";
		case R_liblist_clause: return "liblist_clause";
		case R_library_declaration: return "library_declaration";
		case R_list_of_event_identifiers: return "list_of_event_identifiers";
		case R_list_of_net_decl_assignments_or_identifiers: return "list_of_net_decl_assignments_or_identifiers";
		case R_list_of_port_declarations: return "list_of_port_declarations";
		case R_list_of_ports: return "list_of_ports";
		case R_list_of_variable_port_identifiers: return "list_of_variable_port_identifiers";
		case R_local_parameter_declaration: return "local_parameter_declaration";
		case R_loop_generate_construct: return "loop_generate_construct";
		case R_loop_statement: return "loop_statement";
		case R_mintypmax_expression: return "mintypmax_expression";
		case R_module_declaration: return "module_declaration";
		case R_module_or_udp_instance: return "module_or_udp_instance";
		case R_module_or_udp_instantiation: return "module_or_udp_instantiation";
		case R_module_parameter_port_list: return "module_parameter_port_list";
		case R_module_path_expression: return "module_path_expression";
		case R_module_path_mintypmax_expression: return "module_path_mintypmax_expression";
		case R_mos_switch_instance: return "mos_switch_instance";
		case R_n_input_gate_instance: return "n_input_gate_instance";
		case R_n_output_gate_instance: return "n_output_gate_instance";
		case R_name_of_gate_instance: return "name_of_gate_instance";
		case R_named_parameter_assignment: return "named_parameter_assignment";
		case R_net_assignment: return "net_assignment";
		case R_net_declaration: return "net_declaration";
		case R_net_lvalue: return "net_lvalue";
		case R_number: return "number";
		case R_output_declaration: return "output_declaration";
		case R_par_block: return "par_block";
		case R_parallel_or_full_path_description: return "parallel_or_full_path_description";
		case R_param_assignment: return "param_assignment";
		case R_parameter_declaration: return "parameter_declaration";
		case R_parameter_override: return "parameter_override";
		case R_parameter_value_assignment_or_delay2: return "parameter_value_assignment_or_delay2";
		case R_pass_enable_switch_instance: return "pass_enable_switch_instance";
		case R_pass_switch_instance: return "pass_switch_instance";
		case R_path_delay_value: return "path_delay_value";
		case R_port: return "port";
		case R_port_connection_or_output_terminal: return "port_connection_or_output_terminal";
		case R_port_expression: return "port_expression";
		case R_port_reference: return "port_reference";
		case R_procedural_continuous_assignments: return "procedural_continuous_assignments";
		case R_procedural_timing_control_statement: return "procedural_timing_control_statement";
		case R_pull_gate_instance: return "pull_gate_instance";
		case R_pulldown_strength: return "pulldown_strength";
		case R_pullup_strength: return "pullup_strength";
		case R_pulse_control_specparam: return "pulse_control_specparam";
		case R_pulsestyle_declaration: return "pulsestyle_declaration";
		case R_range: return "range";
		case R_range_expression: return "range_expression";
		case R_real_declaration: return "real_declaration";
		case R_real_type: return "real_type";
		case R_realtime_declaration: return "realtime_declaration";
		case R_reg_declaration: return "reg_declaration";
		case R_scalar_timing_check_condition: return "scalar_timing_check_condition";
		case R_seq_block: return "seq_block";
		case R_sequential_or_combinatorial_entry: return "sequential_or_combinatorial_entry";
		case R_showcancelled_declaration: return "showcancelled_declaration";
		case R_simple_or_edge_sensitive_path_declaration: return "simple_or_edge_sensitive_path_declaration";
		case R_simple_or_edge_sensitive_path_description: return "simple_or_edge_sensitive_path_description";
		case R_simple_path_declaration: return "simple_path_declaration";
		case R_specify_block: return "specify_block";
		case R_specify_input_or_output_terminal_descriptor: return "specify_input_or_output_terminal_descriptor";
		case R_specify_input_terminal_descriptor: return "specify_input_terminal_descriptor";
		case R_specify_output_terminal_descriptor: return "specify_output_terminal_descriptor";
		case R_specparam_assignment: return "specparam_assignment";
		case R_specparam_declaration: return "specparam_declaration";
		case R_state_dependent_path_declaration: return "state_dependent_path_declaration";
		case R_system_task_enable: return "system_task_enable";
		case R_task_declaration: return "task_declaration";
		case R_tf_inout_declaration: return "tf_inout_declaration";
		case R_tf_input_declaration: return "tf_input_declaration";
		case R_tf_output_declaration: return "tf_output_declaration";
		case R_time_declaration: return "time_declaration";
		case R_timing_check_event: return "timing_check_event";
		case R_udp_body: return "udp_body";
		case R_udp_declaration: return "udp_declaration";
		case R_udp_declaration_port_list: return "udp_declaration_port_list";
		case R_udp_initial_statement: return "udp_initial_statement";
		case R_udp_input_declaration: return "udp_input_declaration";
		case R_udp_output_declaration: return "udp_output_declaration";
		case R_udp_port_list: return "udp_port_list";
		case R_udp_reg_declaration: return "udp_reg_declaration";
		case R_use_clause: return "use_clause";
		case R_variable_assignment: return "variable_assignment";
		case R_variable_lvalue: return "variable_lvalue";
		case R_variable_or_net_lvalue: return "variable_or_net_lvalue";
		case R_variable_type: return "variable_type";
		case R_wait_statement: return "wait_statement";
	default: if(r<R_First) return tokenName(r); else return "";
}}
