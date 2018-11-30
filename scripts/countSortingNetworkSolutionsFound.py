
import argparse, os, copy, errno, csv

parser = argparse.ArgumentParser(description="Data aggregation script.")
parser.add_argument("data_file", type=str, help="Target data file")
args = parser.parse_args()

fpath = args.data_file

file_content = None
with open(fpath, "r") as fp:
    file_content = fp.read().strip().split("\n")

header = file_content[0].split(",")
header_lu = {header[i].strip():i for i in range(0, len(header))}
file_content = file_content[1:]

solutions = [l for l in csv.reader(file_content, quotechar='"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)]

info_by_treatment = {}
for sol in solutions:
    treatment = sol[header_lu["treatment"]]
    if not treatment in info_by_treatment:
        info_by_treatment[treatment] = {"solutions_found":0, "total_runs":0}
    if (sol[header_lu["solution_found"]] == "True"):
        info_by_treatment[treatment]["solutions_found"] += 1
    info_by_treatment[treatment]["total_runs"] += 1
    info_by_treatment[treatment]["uses_cohorts"] = sol[header_lu["uses_cohorts"]]

solutions_summary = "treatment,uses_cohorts,solutions_found,total_runs\n"
for treatment in info_by_treatment:
    info = info_by_treatment[treatment]
    solutions_summary += ",".join(map(str, [treatment, info["uses_cohorts"], info["solutions_found"], info["total_runs"]])) + "\n"

new_name = fpath.split("/")[-1].strip(".csv") + "__solutions_summary.csv"
with open(new_name, "w") as fp:
    fp.write(solutions_summary)   
    