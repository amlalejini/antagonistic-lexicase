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

    args = parser.parse_args()

    data_directory = args.data_directory
    dump = args.dump_directory

    # Get a list of all runs
    runs = [d for d in os.listdir(data_directory) if "__" in d]
    runs.sort()

    mkdir_p(dump)

    print("Pulling smallest network solutions from all runs in {}".format(data_directory))
    #update,network_id,fitness,pass_total,network_size,num_antagonists,sorts_per_antagonist,network
    solutions_content = "treatment,run_id,network_size,update_found,fitness,num_antagonists,sorts_per_antagonist,network\n"
    for run in runs:
        print("Run: {}".format(run))
        run_dir = os.path.join(data_directory, run)
        run_id = run.split("__")[-1]
        run = "__".join(run.split("__")[:-1])
        treatment = run
        run_sols = os.path.join(run_dir, "output", "solutions.csv")
        
        file_content = None
        with open(run_sols, "r") as fp:
            file_content = fp.read().split("\n")
        header = file_content[0].split(",")
        header_lu = {header[i].strip():i for i in range(0, len(header))}
        file_content = file_content[1:]
        
        # solutions = [line.split(",") for line in file_content]
        solutions = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]

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
        solutions_content += ",".join([treatment, run_id, network_size, update_found, fitness, num_antagonists, sorts_per_antagonist,'"'+network+'"']) + "\n"
    
    with open(os.path.join(dump, "min_networks.csv"), "w") as fp:
        fp.write(solutions_content)

if __name__ == "__main__":
    main()