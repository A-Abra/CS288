#!/usr/bin/python
import urllib.request
from urllib.request import urlopen
from bs4 import BeautifulSoup
import re

letters = ["A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"]

#Lists for AA to AD
asml1 = ["A"]
asml2 = ["A","B","C","D"]

occurrences = dict()
wordList = []

for let1 in range(len(letters)):
#for let1 in range(len(asml1)):
    for let2 in range(len(letters)):
    #for let2 in range(len(asml2)):
        page = letters[let1] + letters[let2]
        #page = asml1[let1] + asml2[let2]
        file = urllib.request.urlopen("http://en.wikipedia.org/wiki/" + page)
        str_html = file.read()
        obj_html = BeautifulSoup(str_html, "html.parser")
        # rm all script and style elements
        for element in obj_html(["script", "style"]):
            element.extract()
        str_text = obj_html.get_text()
        
        regex = '[a-zA-Z]+'
        for match in re.finditer(regex, str_text):
            word = match.group()
            wordList.append(word)

for word in wordList:
    word = word.lower()
    if word in occurrences:
        occurrences[word] = occurrences[word] + 1
    else:
        occurrences[word] = 1

printTimes = 0
sortWords = sorted(occurrences, key=occurrences.get, reverse=True)
for i in sortWords:
    print(occurrences[i],i)
    printTimes += 1
    if printTimes == 15:
        break
