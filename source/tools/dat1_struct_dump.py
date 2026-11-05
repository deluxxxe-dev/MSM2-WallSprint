"""Read-only DAT1 config struct dumper (WS research tool): prints nested field names/values.
Names come from the file string table; widths: u8/i8/float32, 0x0d = nested struct. HYPOTHESIS-level layout."""
import sys,struct
TAG=bytes.fromhex('44001503')
def a4(x): return (x+3)&~3
def parse(b,i,names):
    assert b[i:i+4]==TAG
    cnt,size=struct.unpack_from('<II',b,i+4);p=i+12
    ent=[struct.unpack_from('<II',b,p+8*k) for k in range(cnt)];p+=8*cnt
    no=struct.unpack_from('<%dI'%cnt,b,p);p+=4*cnt
    cur=a4(p);out={}
    for (h,fl),o in zip(ent,no):
        nm=names(o);t=fl>>24
        if t==8: out[nm]=round(struct.unpack_from('<f',b,cur)[0],4);cur+=4
        elif t==4: out[nm]=struct.unpack_from('<b',b,cur)[0];cur+=1
        elif t==0: out[nm]=b[cur];cur+=1
        elif t==0xd:
            j=b.find(TAG,cur);v,end=parse(b,j,names);out[nm]=v;cur=a4(end)
        else: out[nm]='?fl%x'%fl;cur+=4
    return out,cur
for path in sys.argv[1:]:
    b=open(path,'rb').read()
    tags0=[];i0=0
    while (i0:=b.find(TAG,i0))>=0: tags0.append(i0);i0+=4
    # base of the name table: a leaf X/Y/Z struct's first name offset points at "X\0Y\0Z"
    k=None
    for t in tags0:
        c=struct.unpack_from('<I',b,t+4)[0]
        if c==3:
            o=struct.unpack_from('<I',b,t+12+24)[0];x=b.find(b'X\x00Y\x00Z\x00');k=x-o;break
    def names(o,b=b,k=k):
        # find base: names offsets are relative to start of string table ("Config")
        s=b[k+o:b.index(b'\x00',k+o)];return s.decode(errors='replace')
    # top-level config struct = the TAG with largest count after the type-name struct
    tags=[];i=0
    while (i:=b.find(TAG,i))>=0: tags.append(i);i+=4
    # choose second tag (first is header Type string struct)
    res,_=parse(b,tags[1],names)
    print('==',path.split('/')[-1].split('-',1)[1]);print(' ',res)
