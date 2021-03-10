import json

with open("pagerank.json", "r") as pagerank_file:
    pagerank = json.load(pagerank_file)

i = 1

f = open("pagerank.txt", "w")

for key, value in pagerank.items():
    f.write(str(i) + " " + str(value) + "\n")
    i+=1

f.close()