import argparse, os, copy, errno, csv


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
    parser.add_argument("update", type=int, help="max update to look at distribution")

    args = parser.parse_args()

    data_directory = args.data_directory
    dump = args.dump_directory
    
    print("Pulling test case distributions in for-loop-index problem from {}".format(data_directory))

    mkdir_p(dump)

    # Get a list of all runs
    runs = [d for d in os.listdir(data_directory) if d.strip("PROBLEM_").split("__")[0] == "for-loop-index"]
    runs.sort()

    for run in runs:
        print("Run: {}".format(run))
        run_dir = os.path.join(data_directory, run)
        run_id = run.split("__")[-1]
        run = "__".join(run.split("__")[:-1])
        treatment = run

        pop_dir = os.path.join(run_dir, "output", "pop_{}".format(args.update))
        if not os.path.isdir(pop_dir):
            print("  pop_{} dir not found".format(args.update))
            continue
        
        pop_fpath = os.path.join(pop_dir, "test_pop_{}.csv".format(args.update))
        file_content = None
        with open(pop_fpath, "r") as fp:
            csvreader = csv.reader(fp, delimiter=',', quotechar='"')
        next(csvreader, None)
        tests = [map(int,row[-1].split(",")) for row in csvreader]
        print(str(tests))
        exit()

if __name__ == "__main__":
    main()



