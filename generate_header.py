import json
import os
import sys

if len(sys.argv) < 3:
    sys.exit("<input> or <output> positional argument missing\nUsage: <program_name> <input_file> <output_file>")

input_file=sys.argv[1]
output_file=sys.argv[2]
if not os.path.exists(input_file):
    sys.exit("Input file not exist")

    
with open(input_file, 'r') as in_file:
    data=json.load(in_file)
    algorithms=data["algorithms"]
    
    header_file=open(output_file,"w+")
    header_file.write("""// This header file is auto generated by python script
#ifndef _ALGOS_HPP_
#define _ALGOS_HPP_
""")
    # for algorithm in algorithms:
    #     header_file.write("#include \""+str(algorithm.get('algorithm_file_name'))+"\"")
    
    header_file.write("""
struct algo_info
{
    std::string name;
    bool is_multiple_destination;
    bool is_multiple_source;
    bool completeness;
    void (*algo_function_ptr)(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect);
};

""")
    
    
    for algorithm in algorithms:
        function_name=algorithm.get("algorithm_function_name")
        header_file.write("extern void %s(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect);\n"%(function_name))
    
    
    i=0
    comma=","
    header_file.write("\nstd::vector<struct algo_info> algos{\n")
    for algorithm in algorithms:
        if i==len(algorithms)-1:
            comma=' '

        name=algorithm.get("name")
        if algorithm.get("support_multiple_destination"):
            multi_dest="true"
        else:
            multi_dest="false"
            
        if algorithm.get("support_multiple_source"):
            multi_src="true"
        else:
            multi_src="false"
        
        if algorithm.get("completeness"):
            completeness="true"
        else:
            completeness="false"
        algo_ptr=algorithm.get("algorithm_function_name")
        
        header_file.write("""    {"%s" , %s , %s, %s , %s} %s
"""%(name,multi_dest,multi_src,completeness,algo_ptr,comma))
    i+=1
    
    header_file.write("};\n")
    
    header_file.write("#endif")
    header_file.close()
    