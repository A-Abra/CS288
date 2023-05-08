#!/bin/bash

for page in {A..Z}{A..Z}
#for page in A{A..D}
do
	wget https://en.wikipedia.org/wiki/$page -O $page.html
	lynx -dump -nolist $page.html > $page.txt
	cat $page.txt | grep -o "[a-zA-Z]\+" | tr " " "\n" >> TopWords.txt
	rm -f $page.txt $page.html
done

cat TopWords.txt | sort | uniq -ic | sort -nr | head -n 15 | sed 's/^ *//1'
rm -f TopWords.txt
