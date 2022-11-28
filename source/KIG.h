/**
 * @file 
*/

#ifndef KIG_H
#define KIG_H
#endif

#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cassert>
#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector> 
#include <numeric>
#include <tuple>
#include <filesystem>
#include <limits>
#include <toml.hpp>

/**
 *  @brief This data structure contains the information fetched from the
 *  TOML configuration file. To assure simplicity and continuity, field names are replicating the keys of the TOML file.
 *  @author Francesco Minarini
*/
struct HWconfig {
    
    //this structure will hold the details defined in the dedicated TOML file
    //the names of the variables replicate the fields of the aforementioned
    //config file

    int cpu_tdp;                    /**< Thermal design power per chip in Watts (from lscpu)                           */
    int n_cpu;                      /**< Number of CPU on board                                                        */
    int clock_ticks;                /**< Clock ticks for conversion from jiffies to seconds (from getconf CLK_TCK)     */
    double carbon_intensity;        /**< Carbon cost (by region) of producing electrical power                         */
    double pue;                     /**< Power usage effectiveness metric for the PC or computing facility             */
    double ram_power_usage;         /**< Watts used by RAM                                                             */
    std::string arch;               /**< Architecture of CPU on board                                                  */
    std::string root_folder;        /**< Path of the folder containing process related data, usually /proc/            */
    std::string cpu_stat_file;      /**< Path of the file containing CPU usage related metrics, usually /stat/         */
    std::string mem_stat_file;      /**< Path of the file containing RAM allocation related metrics, usually /status/  */

};

/**
 *  @brief The data structure containing the information regarding computational
 *  activity drawn from /proc/ level information. Each field replicates the formal
 *  name of /proc/ field 
 *  @author Francesco Minarini
*/
struct CPUsage {

    //the properties of this structure follow the meaningful entries of /proc/pid/stat file
    //for this specific task, fields number 14, 15 and 22 are required.
    //so we will look for [13], [14], [21] positions.
    //For technical continuity and readability, the name will be kept as defined in man7.org
    //uptime is derived from system.
    //be aware that UPTIME is given in SECONDS, while *TIME params are given in
    //SYSTEM CLOCK_TICKS (or jiffies if you want to be edgy).

    double utime;                                   /**< Time used by the process in user mode       */
    double stime;                                   /**< Time used by the process in kernel mode     */
    double starttime;                               /**< Time at which the job was started           */
    double up_time;                                 /**< Time elapsed since last boot                */
    double elapsed_time;                            /**< Time elapsed, in seconds, by computation    */
    double vm_size;                                 /**< Size in kB of allocated RAM                 */

};

void pullConfig (HWconfig&, std::string);
double fetchMem (std::string);
void fillBuffer(std::vector<std::string>&, std::string);
void update (CPUsage&, std::vector<std::string>&, struct sysinfo);
void flushBuffer(std::vector<std::string>&);//void flushBuffer(CPUsage&);
double CPUusage(CPUsage&, HWconfig&);
double carbonFootprint(std::vector<double>&, std::vector<double>&, HWconfig&, double);
void makeReport(HWconfig&, double, double);
std::ostream& operator<< (std::ostream& of, std::vector<double>);

