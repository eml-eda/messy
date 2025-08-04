modelName = 'messy_diagram';
new_system(modelName);
open_system(modelName);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Core
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
add_block('simulink/Commonly Used Blocks/Subsystem', [modelName '/Core']);
set_param([modelName '/Core'], 'ContentPreviewEnabled', 'off');
delete_line([modelName '/Core'], 'In1/1', 'Out1/1');
delete_block([modelName '/Core/In1']);
delete_block([modelName '/Core/Out1']);
add_block('simulink/Commonly Used Blocks/In1', [modelName '/Core/i_done_functional_bus']);
add_block('simulink/Commonly Used Blocks/In1', [modelName '/Core/i_data_ptr']);
add_block('simulink/Commonly Used Blocks/In1', [modelName '/Core/i_idx_sensor']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_address']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_data_ptr']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_size']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_is_read']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_activate_functional_bus']);
add_block('simulink/Commonly Used Blocks/Out1', [modelName '/Core/o_power_state']);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functional Bus
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
add_block('simulink/Commonly Used Blocks/Subsystem', [modelName '/FunctionalBus']);
set_param([modelName '/FunctionalBus'], 'ContentPreviewEnabled', 'off');
delete_line([modelName '/FunctionalBus'], 'In1/1', 'Out1/1');
delete_block([modelName '/FunctionalBus/In1']);
delete_block([modelName '/FunctionalBus/Out1']);



Simulink.BlockDiagram.arrangeSystem(modelName, FullLayout='true');