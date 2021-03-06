#ifndef __VLSYNTREE__
#define __VLSYNTREE__
// This file was automatically generated by VerilogEbnf; don't modify it!

#include <Verilog/VlToken.h>
#include <QList>

namespace Vl {

	struct SynTree {
		enum ParserRule {
			R_First = Tok_Eof + 1,
			R_Attribute,
			R_MacroUsage,
			R_always_construct,
			R_block_integer_declaration,
			R_block_real_declaration,
			R_block_real_type,
			R_block_realtime_declaration,
			R_block_reg_declaration,
			R_block_time_declaration,
			R_block_variable_type,
			R_blocking_or_nonblocking_assignment_or_task_enable,
			R_case_generate_construct,
			R_case_generate_item,
			R_case_item,
			R_case_statement,
			R_cell_clause,
			R_charge_strength,
			R_cmos_switch_instance,
			R_conditional_statement,
			R_config_declaration,
			R_config_rule_statement,
			R_constant_expression,
			R_constant_mintypmax_expression,
			R_constant_range_expression,
			R_continuous_assign,
			R_controlled_timing_check_event,
			R_default_clause,
			R_defparam_assignment,
			R_delay,
			R_delay_or_event_control,
			R_delayed_data,
			R_delayed_reference,
			R_design_statement,
			R_dimension,
			R_disable_statement,
			R_dlr_fullskew_timing_check,
			R_dlr_hold_timing_check,
			R_dlr_nochange_timing_check,
			R_dlr_period_timing_check,
			R_dlr_recovery_timing_check,
			R_dlr_recrem_timing_check,
			R_dlr_removal_timing_check,
			R_dlr_setup_timing_check,
			R_dlr_setuphold_timing_check,
			R_dlr_skew_timing_check,
			R_dlr_timeskew_timing_check,
			R_dlr_width_timing_check,
			R_drive_strength,
			R_edge_control_specifier,
			R_edge_descriptor,
			R_enable_gate_instance,
			R_event_control,
			R_event_declaration,
			R_event_expression,
			R_event_trigger,
			R_expression,
			R_expression_2,
			R_file_path_spec,
			R_function_declaration,
			R_function_range_or_type,
			R_gate_instantiation,
			R_generate_block,
			R_generate_region,
			R_genvar_declaration,
			R_genvar_expression,
			R_genvar_initialization,
			R_genvar_iteration,
			R_hierarchical_identifier,
			R_hierarchical_identifier_range,
			R_hierarchical_identifier_range_const,
			R_if_generate_construct,
			R_include_statement,
			R_initial_construct,
			R_inout_declaration,
			R_input_declaration,
			R_inst_clause,
			R_inst_name,
			R_integer_declaration,
			R_liblist_clause,
			R_library_declaration,
			R_list_of_event_identifiers,
			R_list_of_net_decl_assignments_or_identifiers,
			R_list_of_port_declarations,
			R_list_of_ports,
			R_list_of_variable_port_identifiers,
			R_local_parameter_declaration,
			R_loop_generate_construct,
			R_loop_statement,
			R_mintypmax_expression,
			R_module_declaration,
			R_module_or_udp_instance,
			R_module_or_udp_instantiation,
			R_module_parameter_port_list,
			R_module_path_expression,
			R_module_path_mintypmax_expression,
			R_mos_switch_instance,
			R_n_input_gate_instance,
			R_n_output_gate_instance,
			R_name_of_gate_instance,
			R_named_parameter_assignment,
			R_net_assignment,
			R_net_declaration,
			R_net_lvalue,
			R_number,
			R_output_declaration,
			R_par_block,
			R_parallel_or_full_path_description,
			R_param_assignment,
			R_parameter_declaration,
			R_parameter_override,
			R_parameter_value_assignment_or_delay2,
			R_pass_enable_switch_instance,
			R_pass_switch_instance,
			R_path_delay_value,
			R_port,
			R_port_connection_or_output_terminal,
			R_port_expression,
			R_port_reference,
			R_procedural_continuous_assignments,
			R_procedural_timing_control_statement,
			R_pull_gate_instance,
			R_pulldown_strength,
			R_pullup_strength,
			R_pulse_control_specparam,
			R_pulsestyle_declaration,
			R_range,
			R_range_expression,
			R_real_declaration,
			R_real_type,
			R_realtime_declaration,
			R_reg_declaration,
			R_scalar_timing_check_condition,
			R_seq_block,
			R_sequential_or_combinatorial_entry,
			R_showcancelled_declaration,
			R_simple_or_edge_sensitive_path_declaration,
			R_simple_or_edge_sensitive_path_description,
			R_simple_path_declaration,
			R_specify_block,
			R_specify_input_or_output_terminal_descriptor,
			R_specify_input_terminal_descriptor,
			R_specify_output_terminal_descriptor,
			R_specparam_assignment,
			R_specparam_declaration,
			R_state_dependent_path_declaration,
			R_system_task_enable,
			R_task_declaration,
			R_tf_inout_declaration,
			R_tf_input_declaration,
			R_tf_output_declaration,
			R_time_declaration,
			R_timing_check_event,
			R_udp_body,
			R_udp_declaration,
			R_udp_declaration_port_list,
			R_udp_initial_statement,
			R_udp_input_declaration,
			R_udp_output_declaration,
			R_udp_port_list,
			R_udp_reg_declaration,
			R_use_clause,
			R_variable_assignment,
			R_variable_lvalue,
			R_variable_or_net_lvalue,
			R_variable_type,
			R_wait_statement,
			R_Last
		};
		SynTree(quint16 r = Tok_Invalid, const Token& = Token() );
		SynTree(const Token& t ):d_tok(t){}
		~SynTree() { foreach(SynTree* n, d_children) delete n; }

		static const char* rToStr( quint16 r );

		Vl::Token d_tok;
		QList<SynTree*> d_children;
	};

}
#endif // __VLSYNTREE__
