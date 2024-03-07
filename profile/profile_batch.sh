#!/bin/bash

BATCH_UUID="$(uuid)"

{

###############################################################################
echo "SETUP"
echo "########################################################################"
cd "$(dirname "$0")"
echo "pwd $(pwd)"

echo "BATCH_UUID ${BATCH_UUID}"
mkdir -p "batch=${BATCH_UUID}+what=rawdata"

VENV="$(mktemp -d)"
python3 -m venv "${VENV}"
source "${VENV}/bin/activate"
python3 -m pip install --upgrade pip
python3 -m pip install -r requirements.txt


###############################################################################
echo "MAKE PROVLOG"
echo "########################################################################"
  cat << EOF >> "batch=${BATCH_UUID}+what=provlog+ext=.yaml"
-
  a: "batch=${BATCH_UUID}+what=provlog+ext=.yaml"
  batch: ${BATCH_UUID}
  date: \$(date --iso-8601=seconds)
  python: $(python3 --version)
  revision: $(git rev-parse HEAD)
  hostname: $(hostname)
  pwd: $(pwd)
  neofetch:
$(neofetch --stdout | sed 's/^/    /')
  os_info: |
$(lsb_release -a 2>/dev/null || cat /etc/*release | sed 's/^/    /')
  cpu_info: |
$(lscpu | sed 's/^/    /')
  memory_info: |
$(free -h | sed 's/^/    /')
  disk_info: |
$(df -h | sed 's/^/    /')
  network_info: |
$(ifconfig -a | sed 's/^/    /')
  user: $(whoami)
  uuid: $(uuidgen)
  slurm_job_id: ${SLURM_JOB_ID-none}
  profile_batch.sh: |
$(sed 's/^/    /' profile_batch.sh)
  profile_job.py: |
$(sed 's/^/    /' profile_job.py)
  requirements.txt: |
$(sed 's/^/    /' requirements.txt)
EOF

###############################################################################
echo "RUN JOBS"
echo "########################################################################"
for replicate in {0..4}; do for pop_size in 10 1000 100000; do
    JOB_UUID="$(uuid)"
    echo "JOB_UUID ${JOB_UUID} replicate ${replicate} pop_size ${pop_size}"
    mprof run -o "batch=${BATCH_UUID}+what=rawdata/a=memprof+job=${JOB_UUID}+ext=.dat" \
        python3 profile_job.py \
        "${BATCH_UUID}" "${JOB_UUID}" \
        "${replicate}" "${pop_size}"
done; done


###############################################################################
echo "COLLLATE DATA"
echo "########################################################################"
python3 << EOF
import glob

from keyname import keyname as kn
import pandas as pd


# COLLATE TIME DATA
###############################################################################
time_file_list = glob.glob("batch=${BATCH_UUID}+what=rawdata/a=timeprof+job=*+ext=.csv")
time_concat = pd.concat([pd.read_csv(f) for f in time_file_list])
time_concat.to_csv("batch=${BATCH_UUID}+what=timeprof+ext=.csv", index=False)

# COLLATE MEMORY DATA
##############################################################################
mem_file_list = glob.glob("batch=${BATCH_UUID}+what=rawdata/a=memprof+job=*+ext=.dat")

# Read and concatenate the CSV files
mem_data = []
for f in mem_file_list:
    job = kn.unpack(f)["job"]
    mem_df = pd.read_csv(
        f,
        index_col=False,
        names=["MEM", "memory (MiB)", "timestamp"],
        sep=" ",
        skiprows=1,
    )
    mem_df["job"] = job
    mem_df["seconds"] = mem_df["timestamp"] - mem_df["timestamp"].min()

    mem_data.append(mem_df)

mem_concat = pd.concat(mem_data)
mem_merged = mem_concat.merge(time_concat, how="left", on="job")

# Write the combined data to a new CSV, without index
mem_merged.to_csv("batch=${BATCH_UUID}+what=memprof+ext=.csv", index=False)
EOF


###############################################################################
echo "PLOT MEMORY DATA"
echo "########################################################################"
python3 << EOF
import glob

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
from teeplot import teeplot as tp


def plot_memprof(*args, **kwargs) -> None:
    plt.gcf().set_size_inches(5, 3)
    ax = sns.lineplot(*args, **kwargs)
    sns.move_legend(ax, "center left", bbox_to_anchor=(1, 1), frameon=False)
    ax.text(
        0.05, 0.95,
        f"n = {kwargs['data']['replicate'].nunique()}",
        horizontalalignment="left", verticalalignment="top",
        transform=ax.transAxes,
    )


data = pd.read_csv(
    "batch=${BATCH_UUID}+what=memprof+ext=.csv",
    index_col=False,
)
for memory in ("memory (MiB)",):
    plt.gca().clear()
    plt.clf()
    tp.tee(
        plot_memprof,
        data=data,
        x="seconds",
        y=memory,
        hue="population size",
        palette="deep",
        style="population size",
        errorbar="se",
        teeplot_outdir="batch=${BATCH_UUID}+what=teeplots",
    )

EOF


###############################################################################
echo "PLOT TIME DATA"
echo "########################################################################"
python3 << EOF
import glob

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
from teeplot import teeplot as tp


def plot_timeprof(*args, **kwargs) -> None:
    plt.gcf().set_size_inches(5, 3)
    ax = sns.lineplot(*args, errorbar="se", **kwargs)
    ax = sns.scatterplot(*args, **kwargs, color="salmon", marker="+")
    ax.set_xscale("log")
    if kwargs["y"] == "generations per second":
        ax.set_yscale("log")
    ax.text(
        0.95, 0.95,
        f"n = {kwargs['data']['replicate'].nunique()}",
        horizontalalignment="right", verticalalignment="top",
        transform=ax.transAxes,
    )


data = pd.read_csv(
    "batch=${BATCH_UUID}+what=timeprof+ext=.csv",
    index_col=False,
)
data["agent evaluations per second"] = (
    data["generations per second"] * data["population size"]
)
for y in "generations per second", "agent evaluations per second":
    plt.gca().clear()
    plt.clf()
    tp.tee(
        plot_timeprof,
        data=data,
        x="population size",
        y=y,
        teeplot_outdir="batch=${BATCH_UUID}+what=teeplots",
    )
EOF


###############################################################################
echo "MAKE TARGZ ARCHIVE"
echo "########################################################################"
tar -czvf "batch=${BATCH_UUID}+ext=.tar.gz" "batch=${BATCH_UUID}"*

} | tee batch=${BATCH_UUID}+what=stdout+ext=.txt
