
# AML

Author: Alexander Myasnikov

mailto:myasnikov.alexander.s@gmail.com

git:https://gitlab.com/amyasnikov/aml

Version: 0.1



### Цели:

* Описать необходимый минимальный набор инструкций, необходимый для трансляции высокоуровнего языка.



### Особенности:

* Процедуры работают только с аргументами, локальными даннами и глобальными функциями.
* Все данные в code  выровнены по u8.
* Все данные в data  выровнены по u8.
* Все данные в stack выровнены по u8.
* Все данные в regs  выровнены по u64.



### TODO:

* Добавить макросы как в assembler.



### Набор инструкций: // TODO

Все инструкции 16-разрядные

```
xxxx xxxx xxxx xxxx
   0    d    v    v   SET(d,a):     Rd = (0 0 0 0 0 0 0 v)
   1    d    a    b   AND(d,a,b):   Rd = Ra & Rb
   2    d    a    b   OR (d,a,b):   Rd = Ra | Rb
   3    d    a    b   XOR(d,a,b):   Rd = Ra ^ Rb
   4    d    a    b   ADD(d,a,b):   Rd = Ra + Rb   + c
   5    d    a    b   SUB(d,a,b):   Rd = Ra - Rb   - c
   6    d    a    b   MULT(d,a,b):  (Rt, Rd) = Ra * Rb
   7    d    a    b   DIV(d,a,b):   Rd = Ra / Rb; Rt = Ra % Rb
   8    d    a    b   LSH(d,a):     Rd = Ra << Rb
   9    d    a    b   RSH(d,a):     Rd = Ra >> Rb
  15    0    d    a   BR(d,a):      RIP = Rd if Ra
  15    1    d    a   NOT(d,a):     Rd = ~Ra
  15    2    d    a   LOAD(d,a):    Rd = M[Ra]              // TODO width
  15    3    d    a   SAVE(d,a):    M[Ra] = Rd              // TODO width
  15    4    d    a   MOV(d,a):     Rd = Ra
  15   15    0    a   CALL(a):      Сохранение текущих регистров. Создание нового фрейма стека
  15   15   15    0   RET():        Восстановление сохраненных регистров
```



### Макросы: // TODO

```
FUNCTION(name)      Сохраняет адрес функции
LABEL(name)         Сохраняет адрес метки
ADDRESS(Ra, name)   Копирует адрес функции (метки) в регистр Ra
```



### Пример кода: // TODO

```
(FUNC sum
  (BLOCK
    (SET ret (INT 1))
    (SET ret (__add $1 $2))
    (RET ret)))

(FUNC main
  (BLOCK
    (SET tmp
      (CALL sum
        (CALL sum 1 2)
        10))
    (CALL print tmp)))
```



### GENERATION

```
CODE:

(BLOCK a b c) ->
  GEN a
  GEN b
  GEN c

(SET name value) ->
  LABEL name
  PUSH 0
  GEN value
  OP_SAVE

(FUNC name body) ->
  ;

(__add arg1 arg2) ->
  PUSH arg1
  OP_LOAD
  PUSH arg2
  OP_LOAD
  OP_ADD


{ <stack>           ; <code>         } -> { <stack>     }
{                   ; PUSH1 b1:opnd1 } -> { opnd1       }
{ b1:opnd1          ; POP1           } -> {             }
{ b8:opnd1          ; LOAD           } -> { M[opnd1]    }
{ b8:opnd1 b1:opnd2 ; SAVE           } -> {             }   M[opnd1] = opnd2
{ b1:opnd2 b1:opnd2 ; ADD1           } -> { opnd1+opnd2 }

```



