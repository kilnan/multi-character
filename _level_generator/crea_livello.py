#generatore di livelli: da una bitmap si ottiene un file di testo come una matrice di numeri
#legend.txt: corrispondenza tra valori a 24 bit ed elementi di gioco
#formato del file di output:
#   - numero righe
#   - numero colonne
#   - matrice

from PIL import Image
import sys

#creo la legenda a mano, dovrei leggerla da un legend.txt!
legenda=[]
default=0
legenda.append(((0,0,0),1))

im = Image.open(sys.argv[1])
nomeFileOut=sys.argv[1][0:sys.argv[1].rindex(".")]+".txt"
out = open(nomeFileOut,"w")

out.write(str(im.size[0])+","+str(im.size[1])+"\n")

for i in list(im.getdata()):
 elemento=default
 for element in legenda:
   print str(i)+","+str(element[0])+"\n"
   if i[0]==element[0][0]:
     elemento=element[1]
     break
 print elemento
 out.write(str(elemento)+",")

