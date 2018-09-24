'''
Script: agg_min_correct_networks.py

For each run, grab smallest correct solution network. If run has none, report none.

'''

import argparse, os, copy, errno, csv

aggregator_dump = "./aggregated_data"

def mkdir_p(path):
    """
    This is functionally equivalent to the mkdir -p [fname] bash command
    """
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

def main():
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("dump_directory", type=str, help="Where to dump this?")
    parser.add_argument("-SM", "--smallest_solution_data", action="store_true", help="Aggregate first solutions (of a particular size) over time")
    parser.add_argument("-PSZ", "--pop_size_over_time", action="store_true", help="Population sizes over time for each run")

    args = parser.parse_args()

    data_directory = args.data_directory
    dump = args.dump_directory

    # Get a list of all runs
    runs = [d for d in os.listdir(data_directory) if "__" in d]
    runs.sort()

    mkdir_p(dump)

    if (args.smallest_solution_data):
        print("Pulling smallest network solutions from all runs in {}".format(data_directory))
        #update,network_id,fitness,pass_total,network_size,num_antagonists,sorts_per_antagonist,network
        solutions_content = "treatment,run_id,network_size,update_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
        smallest_solutions_content = "treatment,run_id,network_size,update_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
        for run in runs:
            print("Run: {}".format(run))
            run_dir = os.path.join(data_directory, run)
            run_id = run.split("__")[-1]
            run = "__".join(run.split("__")[:-1])
            treatment = run
            run_sols = os.path.join(run_dir, "output", "small_solutions.csv")
            
            file_content = None
            with open(run_sols, "r") as fp:
                file_content = fp.read().strip().split("\n")
            header = file_content[0].split(",")
            header_lu = {header[i].strip():i for i in range(0, len(header))}
            file_content = file_content[1:]
            
            solutions = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]
            # Add all solutions to solutions content
            for sol in solutions:
                solutions_content += ",".join([treatment, run_id, sol[header_lu["network_size"]], sol[header_lu["update"]], sol[header_lu["fitness"]], sol[header_lu["num_antagonists"]], sol[header_lu["sorts_per_antagonist"]],'"'+sol[header_lu["network"]]+'"']) + "\n"

            # Add smallest solution to smallest solution doc
            if len(solutions) > 0:
                min_network=0
                for i in range(0, len(solutions)):
                    if solutions[i][header_lu["network_size"]] < solutions[min_network][header_lu["network_size"]]:
                        min_network = i
                min_sol = solutions[min_network]
                network_size = min_sol[header_lu["network_size"]]
                update_found = min_sol[header_lu["update"]]
                fitness = min_sol[header_lu["fitness"]]
                network = min_sol[header_lu["network"]]
                num_antagonists = min_sol[header_lu["num_antagonists"]]
                sorts_per_antagonist = min_sol[header_lu["sorts_per_antagonist"]]
            else:
                network_size = "NONE"
                update_found = "NONE"
                fitness = "NONE"
                network = "NONE"
                num_antagonists = "NONE"
                sorts_per_antagonist = "NONE"
            
            #"treatment,run_id,network_size,update_found,fitness,network,num_antagonists,sorts_per_antagonist\n"
            smallest_solutions_content += ",".join([treatment, run_id, network_size, update_found, fitness, num_antagonists, sorts_per_antagonist,'"'+network+'"']) + "\n"

        with open(os.path.join(dump, "best_networks_ot.csv"), "w") as fp:
            fp.write(solutions_content)
        with open(os.path.join(dump, "min_networks.csv"), "w") as fp:
            fp.write(smallest_solutions_content)

    # Should we gather network pop size info over time?
    if (args.pop_size_over_time):
        print("Pulling population size information over time")
        pop_size_content = "treatment,run_id,network_size,update,fitness\n"
        for run in runs:
            print("Run: {}".format(run))
            run_dir = os.path.join(data_directory, run)
            run_id = run.split("__")[-1]
            run = "__".join(run.split("__")[:-1])
            treatment = run
            # Pull out all pop directories
            out_dir = os.path.join(run_dir, "output")
            pops = [p for p in os.listdir(data_directory) if "pop_" in p]
            pops.sort()
            for pop in pops:
                print("  Extracting from {}".format(pop))
                pop_ud = pop.split("_")[-1]
                pop_dir = os.path.join(out_dir, pop)

                # Pull out network pop snapshot info
                file_content = None
                with open(os.path.join(pop_dir, "network_pop_{}".format(pop_ud)), "r") as fp:
                    file_content = fp.read().strip().split("\n")
                header = file_content[0].split(",")
                header_lu = {header[i].strip():i for i in range(0, len(header))}
                file_content = file_content[1:]

                networks = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]

                for network in networks:
                    #"treatment,run_id,network_size,update,fitness\n"
                    pop_size_content += ",".join([treatment, run_id, network[header_lu["network_size"]], network[header_lu["update"]], network[header_lu["fitness"]]]) + "\n"
        with open(os.path.join(os.path.join(dump, "network_pop_size_ot.csv"))) as fp:
            fp.write(pop_size_content)



if __name__ == "__main__":
    main()