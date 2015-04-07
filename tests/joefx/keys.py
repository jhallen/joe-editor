
KEYS = {
    # Key name	  NORMAL	CONTROL+	SHIFT+		SHIFT+CONTROL+
    'left':	[b'\x1b[D',	b'\x1b[1;5D',	b'\x1b[1;2D',	b'\x1b[1;6D'],
    'right':	[b'\x1b[C',	b'\x1b[1;5C',	b'\x1b[1;2C',	b'\x1b[1;6C'],
    'up':	[b'\x1b[A',	b'\x1b[1;5A',	b'\x1b[1;2A',	b'\x1b[1;6A'],
    'down':	[b'\x1b[B',	b'\x1b[1;5B',	b'\x1b[1;2B',	b'\x1b[1;6B'],
    'pgup':	[b'\x1b[5~',	b'\x1b[5;5~',	b'\x1b[5;2~',	b'\x1b[5;6~'],
    'pgdown':	[b'\x1b[6~',	b'\x1b[6;5~',	b'\x1b[6;2~',	b'\x1b[6,6~'],
    'f1':	[b'\x1b[OP',	b'\x1bO1;5P',	b'\x1bO1;2P',	b'\x1bO1;6P'],
    'f2':	[b'\x1b[OQ',	b'\x1bO1;5Q',	b'\x1bO1;2Q',	b'\x1bO1;6Q'],
    'f3':	[b'\x1b[OR',	b'\x1bO1;5R',	b'\x1bO1;2R',	b'\x1bO1;6R'],
    'f4':	[b'\x1b[OS',	b'\x1bO1;5S',	b'\x1bO1;2S',	b'\x1bO1;6S'],
    'f5':	[b'\x1b[[15~',	b'\x1b[15;5~',	b'\x1b[15;2~',	b'\x1b[15;6~'],
    'f6':	[b'\x1b[[17~',	b'\x1b[17;5~',	b'\x1b[17;2~',	b'\x1b[17;6~'],
    'f7':	[b'\x1b[[18~',	b'\x1b[18;5~',	b'\x1b[18;2~',	b'\x1b[18;6~'],
    'f8':	[b'\x1b[[19~',	b'\x1b[19;5~',	b'\x1b[19;2~',	b'\x1b[19;6~'],
    'f9':	[b'\x1b[[20~',	b'\x1b[20;5~',	b'\x1b[20;2~',	b'\x1b[20;6~'],
    'f10':	[b'\x1b[[21~',	b'\x1b[21;5~',	b'\x1b[21;2~',	b'\x1b[21;6~'],
    'f11':	[b'\x1b[[23~',	b'\x1b[23;5~',	b'\x1b[23;2~',	b'\x1b[23;6~'],
    'f12':	[b'\x1b[[24~',	b'\x1b[24;5~',	b'\x1b[24;2~',	b'\x1b[24;6~'],
    'del':	[b'\x1b[3~',	b'\x1b[3;5~',	b'\x1b[3;2~',	b'\x1b[3;6~'],
    'space':	[b' ',		b'\x00',	None,		b'\x00'],
    'tab':	[b'\x09',	None,		b'\x1b[Z',	b'\x1b[Z'],
    'enter':	[b'\r'],
    'bs':	[b'\x08'],
    'esc':	[b'\x1b'],
    'home':	[b'\x1b[OH'],
    'end':	[b'\x1b[OF'],
}

def getNamedKey(keyname, ctrl, shift):
    mod = (1 if ctrl else 0) | (2 if shift else 0)
    
    if keyname in KEYS:
        data = KEYS[keyname]
        if len(data) <= mod:
            mod = 0
        if data[mod] is None:
            return data[0]
        return data[mod]
    
    return b''

def getKey(char, ctrl, shift):
    if ctrl:
        b = bytearray(1)
        b[0] = ord(char) & 0x1b
        return bytes(b)
    return char.encode('utf-8')

def toctl(s):
    i = 0
    result = b''
    
    ctrl = False
    shift = False
    
    while i < len(s):
        c = s[i]
        
        if c == '{':
            t = s.index('}', i + 1)
            keyname = s[i+1:t]
            
            if '*' in keyname:
                keyname, count = keyname.split('*')
                count = int(count)
            else:
                count = 1
            
            for n in range(count):
                result += getNamedKey(keyname, ctrl, shift)
            
            ctrl = shift = False
            i = t + 1
            continue
        
        if c == '^' and not ctrl:
            ctrl = True
        elif c == '+' and not shift:
            shift = True
        else:
            result += getKey(c, ctrl, shift)
            ctrl = shift = False
        
        i += 1
    
    return result
