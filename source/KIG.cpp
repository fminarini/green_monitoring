# include "KIG.h"
/* ####################################################################
 *  FUNCTION DEFINITIONS:                                             *
  ################################################################### */

/*!
 *  @brief
 *  This function parses the configuration file located at PATH, pulls the required
 *  infrastructure information and pushes data to a HWconfig struct.
 *
 *  @param[in] hw:   An HWconfig object
 *  @param[in] PATH: The path at which the configuration file is located.
 *
 * @details
 * Before running the function, these two conditions are expected to be TRUE:
 * - A valid HWconfig object exists.
 * - A valid PATH has been set.
 * 
 * After the execution, both conditions will still be TRUE.
 */
void pullConfig (HWconfig& hw, std::string PATH) {
    assert(std::filesystem::exists(std::filesystem::path{PATH}));
    
    toml::value config = toml::parse(PATH);
    
    const auto& exp_name = toml::find(config, "owner");
    hw.exp_name = toml::find<std::string>(exp_name, "title");
    
    const auto& infra = toml::find(config, "infrastructure");
    
    hw.root_folder = toml::find<std::string>(infra, "root_folder");
    hw.cpu_stat_file = toml::find<std::string>(infra, "cpu_stat_file");
    hw.mem_stat_file = toml::find<std::string>(infra, "mem_stat_file");
    
    hw.arch = toml::find<std::string>(infra, "cpu_family");
    hw.cpu_tdp = toml::find<int>(infra, "cpu_tdp");
    hw.n_cpu = toml::find<int>(infra, "n_cpu");
    hw.clock_ticks = toml::find<int>(infra, "clock_ticks");

    const auto& energy = toml::find(config, "energy");
    hw.carbon_intensity = toml::find<double>(energy, "carbon_intensity");     
    hw.pue = toml::find<double>(energy, "power_usage_efficiency");
    
    if (toml::find<std::string>(infra, "ram_family")=="COMMON" && 
        toml::find<int>(infra, "ram_slots")==1) {
            hw.ram_power_usage = 3;
    }
}

/*!
 *	@brief
 *  This function reads the file, located at PATH, containing relevant data for the calculation of
 *  Process-wise CPU usage and pushes it inside a buffer.
 *
 *  @param[in] v:    A string vector used as data-buffer
 *  @param[in] PATH: The location of the file, typically "/proc/<pid>/stat"
 
 *	@details
 *	At the end of the execution of this function:
 *  - v.size() is != 0.
 *  - The path is still TRUE.
 */
void fillBuffer (std::vector<std::string>& v, std::string PATH) {
    assert(std::filesystem::exists(std::filesystem::path{PATH}));
    assert(v.size() == 0);

    std::ifstream filestat(PATH);
    std::string stat_line;
    std::getline(filestat, stat_line);
    
    std::istringstream iss(stat_line);
    //iss.ignore(24);
    std::string s;
    while(getline(iss, s, ' ')) { 
        v.push_back(s.c_str());
        //std::cout << s.c_str() << '\n';
    }
    //std::cout << "buffer len: " << c.proc_buffer.size() << '\n';
    //std::cout << "pos 14 " << c.proc_buffer[14] << '\n';
}

/*!
 *  @brief
 *  This function fetches how much RAM is allocated by the process and returns
 *  the value in GB.
 *  
 *  @param [in] PATH_MEM: The path at which the file containing info about allocated
 *                   ram is stored. Typically: "/proc/<pid>/status"
 *
 *  @return vmsize: The size (in GB) of allocated RAM
 *  
 *  @details
 *  the allocated RAM is considered to be the significant parameter for memory energy consumption in [1].
 *  At the time of execution of the function:
 * 		- PATH_MEM should be TRUE.
 *
 * 	At the end of execution of the function:
 * 		- if the monitored process has finished, PATH_MEM will turn FALSE as the PID will get retired.
 *		- otherwise, PATH_MEM will still be TRUE
 * 
 * [1] Lannelongue, L. et al. "Green Algorithms: Quantifying the Carbon Footprint of Computation", Advanced Science, 2021.*
 */

double fetchMem (std::string PATH_MEM) {
    assert(std::filesystem::exists(std::filesystem::path{PATH_MEM}));

    std::ifstream memstat(PATH_MEM);
    std::string mem;
    for (int i=0; i<17; i++) {
        memstat.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::getline(memstat, mem);
    //std::cout << "allocated memory" << mem << '\n';
    
    std::stringstream s (mem);
    std::string temp;
    double vmsize;
    while (!s.eof()) {
        s >> temp;
        if (std::stringstream(temp) >> vmsize) {
                vmsize = vmsize/1000000;
                break;
        }
    }
    return vmsize;
}

/*!
 *  @brief
 *  This function extracts data from CPUsage buffer and uses it to initialize
 *  each member of a CPUsage object.
 *
 *  @param[in] c: A CPUsage object.
 *  @param[in] v: The data buffer used by fillBuffer() function
 *	@details
 *	At the end of the execution of this function:
 *  - c still exists (with updated fields).
 *  - v still exists, but it will be flushed.
 *  - T still exists,
 */

void update (CPUsage& c, std::vector<std::string>& v, struct sysinfo T) {
    assert(v.size() != 0);

    sysinfo(&T);
    c.utime = std::stod(v[13]);//c.proc_buffer[13]);
    c.stime = std::stod(v[14]);
    c.starttime = std::stod(v[21]);
    c.up_time = T.uptime;
    //std::cout << "saving utime: " << c.proc_buffer[13] << '\n';

}

void flushBuffer (std::vector<std::string>& v) {
    
    v.clear();
    //let's keep it this way for the moment, i think here a lambda can easily
    //substitute everything with no problem at all.
}

/*!
 *  @brief
 *  This function converts CPUsage information from jiffies (clock ticks) to seconds and
 *  evaluates the elapsed computing time.
 * 
 *  Starting from the data saved in a CPUsage object, this function converts
 *  jiffies (clock ticks time-unit) to seconds using the internal CLK_TCK parameter. The elapsed computing time 
 *  is then saved into the CPUsage object, and the CPU usage factor is returned.
 * 
 *  @param[in] c: A CPUsage object.
 *  @param[in] hw: An HWconfig object.
 *
 *  @return cpu_usage: A vector containing the sampled cpu usage over computing
 *  time
 * 
 * @details
 * Before execution:
 * 	- c should be TRUE.
 * 	- hw should be TRUE.
 * After execution:
 	- c still TRUE.
 	- hw still TRUE.
 	- cpu_usage should be bound (0,1]. 0 should correspond to a purely sleeping process and therefore is considered not included in the range.
*/

double CPUusage (CPUsage& c, HWconfig& hw) {
    //this function too can be translated to a lambda since it only does a
    //single action. Still, first we build the toy model, then we optimize the
    //structures.
    
    c.starttime = c.starttime/hw.clock_ticks;                      //converting to seconds
    double utime_sec = c.utime / hw.clock_ticks;
    double stime_sec = c.stime / hw.clock_ticks;
    std::cout << "UTIME_JIFF: " << c.utime << '\n';
    std::cout << "UTIME: " << utime_sec << '\n';
    std::cout << "STIME: " << stime_sec << '\n';
    double cpu_occupation = utime_sec + stime_sec;    		   //converting to seconds
    double elapsed_time = (c.up_time - c.starttime);               //measured in seconds
    c.elapsed_time = elapsed_time;
    std::cout << "CLK_TCK: " << hw.clock_ticks;
    std::cout << "OCCUPIED: " << cpu_occupation << '\n';
    std::cout << "ELAPSED: " << elapsed_time << '\n';
    double cpu_usage = cpu_occupation/elapsed_time;             //this is actually a percentage
    std::cout << "CPU usage: " << cpu_usage << '\n';
    return cpu_usage;

}

/*!
 *  @brief
 *  This function uses CPU usage, RAM usage and infrastructure parameters to provide an
 *  estimation of the Carbon footprint of a running process on a Linux environment.
 *  
 *  This function extracts the carbon footprint of the process as defined in [1]
 *  The value returned is measured in gCO2e.
 *  
 *  @param[in] cpu_data: A vector containing the CPU usage factor over the execution
 *  time.
 *  @param[in] mem_data: A vector containing the size of RAM allocated by process
 *  over execution time
 *  @param[in] hw: An HWconfig object.
 *  @param[in] et: The elapsed computing time
 *
 *  @return X: The value in gCO2e of the carbon footprint of process.
 *	
 *  @details
 *  Before execution:
 *		- cpu_data is TRUE and cpu_data.size() == 0
 * 		- mem_data is TRUE and mem_data.size() == 0
 *		- hw is TRUE
 *		- et is TRUE and != 0
 
 * 	After execution:
 *		- The carbon footprint X is returned, it should be a non-zero value.
*/

double carbonFootprint (std::vector<double>& cpu_data, std::vector<double>& mem_data, HWconfig& hw, double et) {
    assert (cpu_data.size() != 0 && mem_data.size() != 0 && et != 0);

    auto n_samples = cpu_data.size();
    
    auto usage = std::accumulate(std::begin(cpu_data),
                                 std::end(cpu_data),
                                 0.);
    
    auto mem_alloc = std::accumulate(std::begin(mem_data),
                                     std::end(mem_data),
                                     0.);
    
    auto avg_CPU_usage = usage/n_samples;
    auto avg_mem_alloc = mem_alloc/n_samples;

    double core_consumption = hw.n_cpu * hw.cpu_tdp * avg_CPU_usage;
    double mem_consumption = avg_mem_alloc * hw.ram_power_usage;
    
    std::cout << "REQUESTED CORES: " << hw.n_cpu << '\n';
    std::cout << "TDP_PER_CORE: " << hw.cpu_tdp << '\n';
    std::cout << "AVG_CPU_USAGE: " << avg_CPU_usage << '\n';
    std::cout << "AVG_MEM_ALLOC (GB): " << avg_mem_alloc << '\n';
    std::cout << "RAM W: " << hw.ram_power_usage << '\n';
    std::cout << "CORE W: " << core_consumption << '\n';
    std::cout << "MEM W: " << mem_consumption << '\n';
    std::cout << "ABSORBED W: " << core_consumption + mem_consumption << '\n';
    std::cout << "ELAPSED T: " << et << '\n';
    std::cout << "PUE: " << hw.pue << '\n';
    
    return hw.carbon_intensity * (et * (core_consumption + mem_consumption) * hw.pue * 0.001);
    
}

/*!
 * @brief This function generates the LaTeX report table for carbon footprint of the experiment. The format of the table is:
 * ***Experiment Name | Hardware Specs | Power | Elapsed time | CO2e(g) | Cost***
 *
 * @param[in] hw: A HWconfig object containing infrastructure data
 * @param[in] et: Elapsed time of the computation
 * @param[in] footprint: The carbon footprint of the computation evaluated in carbonFootprint
 * 
 * @details 
 * The function is OPTIONAL. After running the function \code{.cpp} std::filesystem::exists("report.txt")\endcode will in any case be TRUE. 
 * 
 * In the file, the user will find:
 * - LaTeX package dependencies to be included in LaTeX report (if not already included by the user).
 * - LaTeX code following the specified layout.
 * - One row for each run of our experiment. Data will be reported with 2 decimal precision, where needed.
 */

void makeReport(HWconfig& hw, double et, double footprint) {
	assert(et != 0 && footprint != 0);

	std::string experimental_data = hw.exp_name + " & X & X & "+ 
									std::to_string(et) + 
									" & " + 
									std::to_string(footprint) + 
									" & " + 
									"X \\" +
									"\\";
	
	if (!std::filesystem::exists("report.txt")) {
	
		std::ofstream report;
    	report.open("report.txt");

    	report << "##########################################################" << '\n';
    	report << "IF NOT ALREADY SET, PUT THESE LINES IN YOUR LATEX PREAMBLE" << '\n';
    	report << "##########################################################" << '\n';
    	report << "\\usepackage{array}" << '\n';
    	report << "\\usepackage{textcomp}" << '\n';
    	report << "\\newcolumntype{P}{>{\\centering\\arraybackslash}m{3cm}}"   << '\n';
    	report << "##########################################################" << '\n';

    	report << "\\begin{center}" << '\n';
    	report << "\\begin{tabular}{ c P c c c c }" << '\n';
    	report << "Experiment & Hardware Specs & Power(W) & Elapsed Time(s) & $CO_2e$(g) & Cost(\\texteuro) \\" << "\\" << '\n';
    	report << "\\hline\\hline" << '\n';
		report << experimental_data << '\n';
    	//report << "X & X & X & " << et << " & " << footprint << " & " << "X \\" << "\\" << '\n';
    	report << "\\hline" << '\n';
    	report << "\\end{tabular}" << '\n';
    	report << "\\end{center}" << '\n';

    	report.close();
	}
	else {
		std::fstream read_report;
		read_report.open("report.txt");
		std::vector<std::string> buffer;
		std::string line;
		while (std::getline(read_report, line)) {
			buffer.push_back(line);
		}
		read_report.close();
		
		buffer.insert(std::end(buffer)-2, experimental_data);
		buffer.insert(std::end(buffer)-2, "\\hline");
		
		std::ofstream overwrite_report("report.txt");
		for (int i=0; i<buffer.size(); i++) {
			overwrite_report << buffer[i] << '\n';
		}
		overwrite_report.close();
	}
}

std::ostream& operator<< (std::ostream& of, std::vector<double> v) {
    of << "cpu usage" << '\n';
    for (auto i : v) {
        of << i << ' ';
    }
    of << '\n';
    return of;
}

