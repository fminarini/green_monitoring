#include <KIG.h>
#include <chrono>

#define BUFFER_SIZE 10                                          //adjustable.
std::string config_f = "/conf/config.toml";                  
struct sysinfo s_info;                                          //structure needed to access uptime info

/*-------------------------------------------------------------
 *
 *  This represents the minimal working example for KIG
 *
 * ------------------------------------------------------------*/


int main (int argc, char** argv) {
   
    HWconfig conf;
    pullConfig(conf, config_f);
    CPUsage monitor;
    
    std::vector<std::string> proc_buffer;                        //reading buffer for storing the content of /proc/$pid/stat

    std::vector<double> cpu_usage_buffer;
    cpu_usage_buffer.reserve(BUFFER_SIZE);

    std::vector<double> mem_allocation;
    mem_allocation.reserve(BUFFER_SIZE);

    auto pid = std::string(argv[1]);
    auto path = conf.root_folder + pid + conf.cpu_stat_file;
    auto path_mem = conf.root_folder + pid + conf.mem_stat_file;
    
    std::filesystem::path PROCESS_FOLDER {path};
    
    if (argc == 2) {

        std::cout << "process: " << pid << " is under monitoring" << '\n';
        std::cout << "pulling configuration from: " << path << '\n';

        do {
            fillBuffer(proc_buffer, path);
            mem_allocation.push_back(fetchMem(path_mem));
	    std::cout << mem_allocation << '\n';
            update(monitor, proc_buffer, s_info);
            cpu_usage_buffer.push_back(CPUusage(monitor, conf));
	    std:cout << cpu_usage_buffer << '\n';
            flushBuffer(proc_buffer);
            sleep(10);
        } while (std::filesystem::exists(PROCESS_FOLDER))/*cpu_usage_buffer.size() != BUFFER_SIZE*/;
    
        double e_time = monitor.elapsed_time;
    //std::cout << " This was the cpu usage throughout computing: " << cpu_usage_buffer;
        std::cout << "===============================================" << '\n';
        std::cout << "Now evaluating carbon footprint of execution..." << '\n';
        std::cout << "===============================================" << '\n';
        auto footprint = carbonFootprint(cpu_usage_buffer, mem_allocation, conf, e_time/3600.);
        std::cout << "YOUR FOOTPRINT: " << footprint << " gCO2e" << '\n';
        makeReport(conf, e_time, footprint);
    } 
    
    else {
        std::cout << "monitoring pids: ";
        for(int i=1; i<argc; i++){
            std::cout << argv[i] << ' ';
        }
        std::cout << '\n';

        
        
        auto tock = std::chrono::steady_clock::now();
        
        while(std::filesystem::exists(PROCESS_FOLDER)){
        	for(int i=1; i<argc; i++){
        		
                auto pid_multi = std::string(argv[i]);
                auto path_multi  = conf.root_folder + pid_multi + conf.cpu_stat_file;
                auto path_mem_multi = conf.root_folder + pid_multi + conf.mem_stat_file;
			if(std::filesystem::exists(path_multi)){		
				std::cout << "pulling configuration from: " << path_multi << '\n';
				
                		fillBuffer(proc_buffer, path_multi);
                		mem_allocation.push_back(fetchMem(path_mem_multi));
                		update(monitor, proc_buffer, s_info);
                		cpu_usage_buffer.push_back(CPUusage(monitor,conf));
                		flushBuffer(proc_buffer);
                //congegnare l'equivalente operazione
				}
			else{
				continue;
			} 
            }
            sleep(5);   
        }
       auto tick = std::chrono::steady_clock::now();
       
       std::chrono::duration<double> e_time = tick-tock;
       
       auto cpu_agg = std::accumulate(std::begin(cpu_usage_buffer),
       								  std::end(cpu_usage_buffer),
       								  0.);
       
       auto cpu_mean = cpu_agg / cpu_usage_buffer.size();
       
       auto mem_agg = std::accumulate(std::begin(mem_allocation),
       								  std::end(mem_allocation),
       								  0.);
       
       auto mem_mean = mem_agg / mem_allocation.size();
       
       std::cout << "===============================================" << '\n';
       std::cout << "Now evaluating carbon footprint of execution..." << '\n';
       std::cout << "===============================================" << '\n';
       auto footprint = carbonFootprint(cpu_usage_buffer, 
       									mem_allocation, 
       									conf, 
       									e_time.count()/3600);
       									
       std::cout << "YOUR FOOTPRINT: " << footprint << " gCO2e" << '\n';
       makeReport(conf, e_time.count(), footprint);
    }
}

