import pandas as pd
import numpy as np
import zipfile
import os


curr_path = os.path.dirname(os.path.abspath(__file__))
files_to_preprocess = [f for f in os.listdir(curr_path + "/raw/") if ".txt" in f]

for file in files_to_preprocess:
    df = pd.read_csv(curr_path + f'/raw/{file}', sep='|', index_col=False, header=None)
    df = df[[5, 8]]
    df_no_duplicates = df.drop_duplicates(subset=5, keep='first', ignore_index=True)
    df_no_duplicates.to_csv(curr_path + f"/preprocessed/preprocessed-{file}", sep=" ", index=False, header=False)