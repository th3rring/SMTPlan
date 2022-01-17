#!/bin/python3

import sqlite3

con = sqlite3.connect("example.db")
cur = con.cursor()

# Create a new table if one doesn't exist
cur.execute("CREATE TABLE IF NOT EXISTS experiments (domain text, problem text, date text, duration real)")

# Array of dummy data
dummy_data = [("domain_a", "problem_1", "01-17-2022", 45), ("domain_b", "problem_2", "01-18-2022", 40)]

def data_generator(data):
    for d in data:
        # print(d)
        yield d

# Add some dummy data
cur.executemany("insert into experiments values (?, ?, ?, ?)", data_generator(dummy_data))

# Select all rows and print
cur.execute("select * from experiments")
print(cur.fetchall())

# Save
con.commit()

# Exit
con.close()


