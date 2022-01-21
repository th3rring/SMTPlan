import sqlite3

def data_generator(data):
    for d in data:
        yield d

class DatabaseManager:
    def __init__(self, filename) -> None:
        self.filename = filename
        self.connect = sqlite3.connect(filename)
        self.cursor = self.connect.cursor()

        # Making table if one doesn't exist
        self.cursor.execute("""CREATE TABLE IF NOT EXISTS experiments 
        (domain TEXT, problem TEXT, date TEXT, time TEXT, 
        sat INTEGER, grounded_time REAL, algebra_time REAL, sol_time REAL, iterations INTEGER, timeout INTEGER, total_time REAL, log TEXT)""")

    def insert_expts(self, expt_results):
        self.cursor.executemany("insert into experiments values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", data_generator(expt_results))

    def insert_expt(self, expt_result):
        self.cursor.executemany("insert into experiments values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", data_generator([expt_result]))

    def save(self):
        self.connect.commit()

    def close(self):
        self.connect.close()

    def save_and_close(self):
        self.save()
        self.close()


        

        
