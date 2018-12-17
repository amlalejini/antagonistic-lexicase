'''
Script: agg_min_correct_networks.py

For each run, grab smallest correct solution network. If run has none, report none.

'''

import argparse, os, copy, errno, csv

aggregator_dump = "./aggregated_data"

default_update = 1000000

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
    parser.add_argument("-u", "--update", type=int, help="max update to look for solutions")
    parser.add_argument("-e", "--evaluations", type=int, help="max evaluation to look for solutions")

    args = parser.parse_args()

    data_directory = args.data_directory
    dump = args.dump_directory

    print("Pulling smallest network solutions from all runs in {}".format(data_directory))

    # Get a list of all runs
    runs = [d for d in os.listdir(data_directory) if "__" in d]
    runs.sort()

    mkdir_p(dump)

    if args.update != None:
        update = args.update
        print("Looking for best solutions from update {} or earlier.".format(update))
        
        solutions_content = "treatment,run_id,uses_cohorts,solution_found,solution_size,update_found,evaluation_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
        
        for run in runs:
            print("Run: {}".format(run))
            run_dir = os.path.join(data_directory, run)
            run_id = run.split("__")[-1]
            run = "__".join(run.split("__")[:-2])
            treatment = run
            run_sols = os.path.join(run_dir, "output", "small_solutions.csv")

            uses_cohorts = "1" if "CLEX" in run else "0"
            
            file_content = None
            with open(run_sols, "r") as fp:
                file_content = fp.read().strip().split("\n")

            header = file_content[0].split(",")
            header_lu = {header[i].strip():i for i in range(0, len(header))}
            file_content = file_content[1:]
            
            solutions = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]
            # Add smallest solution to smallest solution doc
            min_network = None
            sol_found = False
            if len(solutions) > 0:
                # Find the smallest network
                for i in range(0, len(solutions)):
                    sol_update = int(solutions[i][header_lu["update"]])
                    if sol_update > update: continue
                    if min_network == None:
                        min_network = i
                        sol_found = True
                    elif float(solutions[i][header_lu["network_size"]]) < float(solutions[min_network][header_lu["network_size"]]):
                        min_network = i
                        sol_found = True
                    
            if sol_found:
                min_sol = solutions[min_network]
                network_size = min_sol[header_lu["network_size"]]
                update_found = min_sol[header_lu["update"]]
                evaluation_found = min_sol[header_lu["evaluations"]]
                fitness = min_sol[header_lu["fitness"]]
                network = min_sol[header_lu["network"]]
                num_antagonists = min_sol[header_lu["num_antagonists"]]
                sorts_per_antagonist = min_sol[header_lu["sorts_per_antagonist"]]
            else:
                network_size = "NONE"
                update_found = "NONE"
                evaluation_found = "NONE"
                fitness = "NONE"
                network = "NONE"
                num_antagonists = "NONE"
                sorts_per_antagonist = "NONE"
            
            #"treatment,run_id,uses_cohorts,solution_found,solution_size,update_found,evaluation_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
            solutions_content += ",".join(map(str,[treatment, run_id, uses_cohorts, sol_found, network_size, update_found, evaluation_found, fitness, num_antagonists, sorts_per_antagonist, '"{}"'.format(network)])) + "\n"
        with open(os.path.join(dump, "min_networks__update_{}.csv".format(update)), "w") as fp:
            fp.write(solutions_content)
    
    if args.evaluations != None:
        evaluation = args.evaluations

        print("Looking for best solutions from evaluation {} or earlier.".format(update))
        
        solutions_content = "treatment,run_id,uses_cohorts,solution_found,solution_size,update_found,evaluation_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
        
        for run in runs:
            print("Run: {}".format(run))
            run_dir = os.path.join(data_directory, run)
            run_id = run.split("__")[-1]
            run = "__".join(run.split("__")[:-1])
            treatment = run
            run_sols = os.path.join(run_dir, "output", "small_solutions.csv")

            uses_cohorts = "1" if "CLEX" in run else "0"
            
            file_content = None
            with open(run_sols, "r") as fp:
                file_content = fp.read().strip().split("\n")

            header = file_content[0].split(",")
            header_lu = {header[i].strip():i for i in range(0, len(header))}
            file_content = file_content[1:]
            
            solutions = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]
            # Add smallest solution to smallest solution doc
            min_network = None
            sol_found = False
            if len(solutions) > 0:
                # Find the smallest network
                for i in range(0, len(solutions)):
                    sol_evaluation = int(solutions[i][header_lu["evaluations"]])
                    if sol_evaluation > evaluation: continue
                    if min_network == None:
                        min_network = i
                        sol_found = True
                    elif float(solutions[i][header_lu["network_size"]]) < float(solutions[min_network][header_lu["network_size"]]):
                        min_network = i
                        sol_found = True
                    
            if sol_found:
                min_sol = solutions[min_network]
                network_size = min_sol[header_lu["network_size"]]
                update_found = min_sol[header_lu["update"]]
                evaluation_found = min_sol[header_lu["evaluations"]]
                fitness = min_sol[header_lu["fitness"]]
                network = min_sol[header_lu["network"]]
                num_antagonists = min_sol[header_lu["num_antagonists"]]
                sorts_per_antagonist = min_sol[header_lu["sorts_per_antagonist"]]
            else:
                network_size = "NONE"
                update_found = "NONE"
                evaluation_found = "NONE"
                fitness = "NONE"
                network = "NONE"
                num_antagonists = "NONE"
                sorts_per_antagonist = "NONE"
            
            #"treatment,run_id,uses_cohorts,solution_found,solution_size,update_found,evaluation_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
            solutions_content += ",".join(map(str,[treatment, run_id, uses_cohorts, sol_found, network_size, update_found, evaluation_found, fitness, num_antagonists, sorts_per_antagonist, '"{}"'.format(network)])) + "\n"
        with open(os.path.join(dump, "min_networks__eval_{}.csv".format(update)), "w") as fp:
            fp.write(solutions_content)

if __name__ == "__main__":
    main()