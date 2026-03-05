.model small
.stack 100h
.data

snake_c db 255 dup(?)
snake_r db 255 dup(?)
Sdir db 0
Sleng dw 1

food_r db ?
food_c db ?

score dw 0
score_msg db "SCORE: $"                               
game_over_msg db "game over! score: $"
restart_msg db "press R to restart$"

.code
MAIN proc 
    
mov ax, @data
mov ds, ax

start:  ;start of game for the restart 

;draw border:
mov ah, 0   ;set video mode 
mov al, 03h ;80x25
int 10h     ;
            
;top row:
mov ax, 0 

corner1:
mov ah, 2  ;move to top-left corner:
mov dh, 2  ;row   
mov dl, 0  ;col
mov bh, 0 
int 10h    

mov ah, 0eh ;the print of corner
mov al, 0DAh 
int 10h
           
mov dl, 1 ;set to col 1
mov ah, 2 ;set pos
mov dh, 2 ;row
int 10h

row1:
inc dl;
mov ah, 0eh
mov al, 0C4h
int 10h
cmp dl, 79
je corner2   ;jump to bottom row
jmp row1 

corner2:
mov ah, 0eh   ;print top-right corner
mov al, 0BFh
int 10h

corner3:
mov ah, 2  ;move pos
mov dh, 24 ;to bottom row   
mov dl, 0
int 10h
push ax

mov ah, 0eh ; print corner
mov al, 0C0h
int 10h
pop ax  ;restore the ah val
mov dl, 1

row2:
inc dl
mov ah, 0eh
mov al, 0C4h
int 10h
cmp dl, 79
je corner4
jmp row2

corner4:
mov ah, 0eh
mov al, 0D9h
int 10h


mov dh, 3 ;set start of row 
col1:
mov ah, 2 ;set pos
mov dl, 0 ;for col1 start 
int 10h
mov ah, 0eh   ;print col1 
mov al, 0B3h
int 10h
inc dh
cmp dh, 24
je BCol2
jmp col1



BCol2:
mov dh, 3

col2:
mov ah, 2
mov dl, 79 ;for co2 start
int 10h
mov ah, 0eh ;print col2
mov al, 0B3h
int 10h
inc dh
cmp dh, 24
je snake
jmp col2

snake:
mov snake_c[0], 40 ;inetialize starting pos
mov snake_r[0], 12
mov ah, 2          ;print starting point
mov dh, snake_r[0]         ;at the middle
mov dl, snake_c[0] 
int 10h
mov ah, 0eh
mov al, '@'
int 10h
mov Sleng, 1  ;set starting length
mov score, 0  ;and score
 
call food  ;inetial food 
call score_dis ;inetial score =0

snake_loop:        ;game start (core)

mov ah, 01h  ;take keystroke
int 16h
jz continue

mov ah, 00h  ;read the key
int 16h

cmp al, 'a'  ;left turn check
je Tleft
cmp al, 'A'
je Tleft

cmp al, 's'  ;right turn check
je Tright
cmp al, 'S'
je Tright

jmp continue

Tleft:
cmp Sdir, 0  ;to loop around
je wrapL
dec Sdir
jmp continue
wrapL:
mov Sdir, 3
jmp continue          

Tright:
cmp Sdir, 3  ;to loop around 
je wrapR
inc Sdir
jmp continue
wrapR:
mov Sdir, 0
           
           
continue:
cmp Sleng, 1     ;erase the body first
jle erase_head
mov si, Sleng
dec si ;point at tail end
mov ah, 2
mov dh, snake_r[si]
mov dl, snake_c[si]
int 10h
mov ah, 0eh
mov al, ' '
int 10h



erase_head:
call body_move   ;proc for body movement
mov si, 0
mov ah, 2
mov dh, snake_r[si]
mov dl, snake_c[si]
int 10h
mov ah, 0eh        ;remove current head
mov al, ' '
int 10h
;movement 
right:
cmp Sdir, 0
jne down 
inc snake_c[si]    ;update pos
jmp Fcheck

down:
cmp Sdir, 1
jne left
inc snake_r[si]    ;update pos
jmp Fcheck

left:
cmp Sdir, 2
jne up
dec snake_c[si]    ;update pos
jmp Fcheck

up:
cmp Sdir, 3
jne right
dec snake_r[si]    ;update pos
jmp Fcheck


Fcheck:               ;eating check:
mov al, snake_c[si] 
cmp al, food_c 
jne coll_check 

mov al, snake_r[si]
cmp al, food_r
jne coll_check

inc Sleng
inc score
call score_dis
call food

coll_check:
call wall_collision
cmp al, 1
je game_over
call self_collision
cmp al, 1
je game_over


draw:
cmp Sleng, 1  ;check if length is >1
jle Dhead     ;only draw head
mov cx, Sleng
mov si, 1
Dbody:
mov ah, 2
mov dh, snake_r[si]
mov dl, snake_c[si]
int 10h
mov ah, 0eh
mov al, 'O'
int 10h
inc si
loop Dbody

Dhead:
mov si, 0
mov ah, 2
mov dh, snake_r[si]    ;draw at new pos 
mov dl, snake_c[si]
int 10h
mov ah, 0eh
mov al, '@'
int 10h

;call delay ; you can add or remove this fcn depends on the speed you want
           ;go to delay proc 
jmp snake_loop ;repeat 

game_over:           ;game over handling 
call game_over_dis

restart_loop:    ;loop to wait for r or R
mov ah, 00h
int 16h
cmp al, 'r'
je restart
cmp al, 'R'
je restart
jmp restart_loop
restart:
jmp start

mov ah, 4ch
int 21h
MAIN endp

;fcn section

food proc    
;divide and take remainder for col
mov ah, 00h
int 1Ah
mov ax, dx
mov dx, 0    ;rem is srored here 
mov bx, 78   ;to remain in the game border 
div bx
inc dl       ;so food doesnt spawn on the border  
mov food_c, dl 

;divide and take remainder for row
mov ah, 00h
int 1Ah
mov ax, dx
mov dx, 0    ;rem is srored here 
mov bx, 21   ;to remain in the game border 
div bx
add dl, 3      ;so food doesnt spawn on the border  
mov food_r, dl 

;food Draw
mov ah, 2
mov dh, food_r
mov dl, food_c
mov bh, 0
int 10h

mov ah, 0eh
mov al, '*'
int 10h 
ret
food endp

delay proc
mov ax, 10  ;change the value of ax to change game speed
l1:
cmp ax, 1
je l2 
dec ax
jmp l1
l2: 
ret 
delay endp

score_dis proc
pusha      ;to save all regs
mov ah, 2  ;move to top left of screen 
mov dh, 0
mov dl, 0
int 10h
mov ah, 9   ;print the score msg
mov dx, offset score_msg
int 21h
mov ax, score
call num


popa
ret
score_dis endp

num proc
pusha
cmp ax, 0   ;special case if score == 0
jne NumNext
mov ah, 0eh
mov al, '0'
int 10h
jmp NumDone ;jump to finish proc

NumNext:
mov cx, 0
mov bx, 10 ;num to div AX by
xor dx, dx
NumDiv: ;loop to get the score 
div bx  ;same as AX/BX to get single digit
push dx ;remainder is stored here single digit 
inc cx  ;count how many digits the score is
cmp ax, 0
je NumPrint
jmp NumDiv
NumPrint: ;loop to print the score
pop dx  
add dl, 30h ;convert to Ascii for printing 
mov ah, 0eh
mov al, dl
int 10h
loop NumPrint
NumDone:    
popa    
ret
num endp   

body_move proc
cmp Sleng, 1
je BodyEnd    
mov si, Sleng
mov cx, Sleng
dec cx
dec si

length_loop:
mov al, snake_c[si-1]
mov snake_c[si], al
mov al, snake_r[si-1]
mov snake_r[si], al
dec si
loop length_loop  
BodyEnd:
ret
body_move endp   

wall_collision proc
cmp snake_c[0], 0   ;wall check for top and bottom
jle wall_hit
cmp snake_c[0], 79
jge wall_hit
cmp snake_r[0], 2  ;wall check for left and right
jle wall_hit
cmp snake_r[0], 24
jge wall_hit

mov al, 0 
ret
wall_hit:
mov al, 1
ret
wall_collision endp 

self_collision proc
push cx
push si
cmp Sleng, 1  ;if there is only a head 
je no_coll
 
mov cx, Sleng
mov si, 1
self_loop:
mov dl, snake_c[0]
cmp snake_c[si], dl
jne next_self
mov dl, snake_r[0]
cmp snake_r[si], dl
je yes_coll          ;if it reaches here it means tere was coll before

next_self:
inc si
cmp si, cx  
jb self_loop 

no_coll:
pop cx          ;collision not found 
pop si
mov al, 0
ret
yes_coll:
pop cx
pop si
mov al, 1 ;collision found
ret
    
self_collision endp 

game_over_dis proc  ;to diplay game over screen
mov ah, 2        ;move to cnter of screen
mov dh, 12
mov dl, 33
int 10h
mov ah, 9
mov dx, offset game_over_msg    ;print the game over msg
int 21h
mov ax, score  ;to print the score at the line end
call num

mov ah, 2        ;move to next line
mov dh, 14
mov dl, 33
int 10h
mov ah, 9
mov dx, offset restart_msg    ;print the restart msg
int 21h

ret
game_over_dis endp

end MAIN
