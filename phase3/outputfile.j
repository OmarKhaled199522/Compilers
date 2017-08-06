.source code
.class public test
.super java/lang/Object

.method public <init>()V
aload_0
invokenonvirtual java/lang/Object/<init>()V
return
.end method

.method public static main([Ljava/lang/String;)V
.limit locals 1024
.limit stack 2048
iconst_0
istore 1
fconst_0
fstore 2
iconst_0
istore 3
Label0:
iconst_0
istore 4
Label1:
bipush 0
istore 3
Label2:
iload 3
bipush 10
if_icmplt Label4
goto Label9
Label3:
iload 3
bipush 1
iadd
istore 3
goto Label2
Label4:
iload 3
istore 0
getstatic      java/lang/System/out Ljava/io/PrintStream;
iload 0
invokevirtual java/io/PrintStream/println(I)V
Label5:
iload 3
istore 4
Label6:
iload 4
iload 3
bipush 10
iadd
if_icmplt Label8
goto Label3
Label7:
iload 4
bipush 1
iadd
istore 4
goto Label6
Label8:
iload 4
istore 0
getstatic      java/lang/System/out Ljava/io/PrintStream;
iload 0
invokevirtual java/io/PrintStream/println(I)V
goto Label7
goto Label3
Label9:
Label10:
iload 4
bipush 5
if_icmpgt Label11
goto Label13
Label11:
iload 4
istore 0
getstatic      java/lang/System/out Ljava/io/PrintStream;
iload 0
invokevirtual java/io/PrintStream/println(I)V
Label12:
iload 4
bipush 1
isub
istore 4
goto Label10
Label13:
iload 4
istore 0
getstatic      java/lang/System/out Ljava/io/PrintStream;
iload 0
invokevirtual java/io/PrintStream/println(I)V
Label14:
return
.end method
