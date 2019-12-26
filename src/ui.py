import tkinter
from tkinter import filedialog
from tkinter import ttk
import json
from ctypes import *
import os
import shutil

class Cpms():
    def __init__(self):
        self.workpath = ''
        self.sourcename = ''
        self.sourceextendname = ''
        self.rulepath = ''
        self.blockcontextpath = ''
        self.subBlockpath = ''

        self.adder = CDLL('./cpms.so')

        window = tkinter.Tk()
        window.title("PECWMA")
        window.resizable(0,0)

        tabControl = ttk.Notebook(window)
        configtab = ttk.Frame(tabControl)  # Create a tab
        tabControl.add(configtab, text='config')  # Add the tab
        mutationtab = ttk.Frame(tabControl)  # Add a second tab
        tabControl.add(mutationtab, text='mutant generation')  # Make second tab visible
        combinetab = ttk.Frame(tabControl)  # Add a third tab
        tabControl.add(combinetab, text='systhesis')  # Make third tab visible
        testtab = ttk.Frame(tabControl)  # Add a fourth tab
        tabControl.add(testtab, text='test')  # Make fourth tab visible

        tabControl.pack(expand=1, fill="both")  # Pack to make visible

        menubar = tkinter.Menu(window)
        window.config(menu=menubar)
        # file menu
        # filemenu = tkinter.Menu(menubar, tearoff=0)
        # filemenu.add_command(label="import", command=self.open_file)
        # menubar.add_cascade(label="file", menu=filemenu)
        '''
        config part
        '''
        # LabelFrame using tab1 as the parent
        configFrame = ttk.LabelFrame(configtab)
        configFrame.grid(column=0, row=0, padx=8, pady=4)
        #path config
        pathFrame=ttk.LabelFrame(configFrame,text='path')
        pathFrame.grid(column=0, row=0, padx=8, pady=4,columnspan=3)
        rootlabel = tkinter.Label(pathFrame, text="rootpath")
        rootlabel.grid(row=2, column=0)
        self.rootpathentry = tkinter.Entry(pathFrame, width=37, font=("宋体", 10, 'bold'))
        self.rootpathentry.grid(row=2, column=1)
        rootbutton = tkinter.Button(pathFrame, text="select", command=lambda: self.selectfolder(self.rootpathentry))
        rootbutton.grid(row=2, column=2)

        sourcelabel = tkinter.Label(pathFrame, text="sourcepath")
        sourcelabel.grid(row=3, column=0)
        self.sourcepathentry = tkinter.Entry(pathFrame, width=37, font=("宋体", 10, 'bold'))
        self.sourcepathentry.grid(row=3, column=1)
        sourcebutton = tkinter.Button(pathFrame, text="select", command=lambda: self.selectfile(self.sourcepathentry))
        sourcebutton.grid(row=3, column=2)

        mutantslabel = tkinter.Label(pathFrame, text="mutantspath")
        mutantslabel.grid(row=4, column=0)
        self.mutantspathentry = tkinter.Entry(pathFrame, width=37, font=("宋体", 10, 'bold'))
        self.mutantspathentry.grid(row=4, column=1)
        mutantsbutton = tkinter.Button(pathFrame, text="select", command=lambda: self.selectfolder(self.mutantspathentry))
        mutantsbutton.grid(row=4, column=2)

        testcastlabel = tkinter.Label(pathFrame, text="testcasepath")
        testcastlabel.grid(row=6, column=0)
        self.testcasepathentry = tkinter.Entry(pathFrame, width=37, font=("宋体", 10, 'bold'))
        self.testcasepathentry.grid(row=6, column=1)
        testcasebutton = tkinter.Button(pathFrame, text="select",
                                        command=lambda: self.selectfolder(self.testcasepathentry))
        testcasebutton.grid(row=6, column=2)
        # show block rule
        ruleFrame=ttk.LabelFrame(configFrame, text='block rule')
        ruleFrame.grid(column=0, row=1, padx=8, pady=4,sticky='W',rowspan=3)
        rulescrollbar = tkinter.Scrollbar(ruleFrame)
        rulescrollbar.grid(row=8, column=2, sticky='ns')
        self.rulelistbox = tkinter.Listbox(ruleFrame, width=27,yscrollcommand=rulescrollbar.set)
        self.rulelistbox.grid(row=8, column=1,columnspan=4,sticky='W')
        rulescrollbar.config(command=self.rulelistbox.yview)
        # confirm and import button
        self.imports = tkinter.Button(configFrame, text="import", command=self.imports)
        self.imports.grid(row=2, column=1, sticky='w')
        konglabel=tkinter.Label(configFrame, text=' ')
        konglabel.grid(row=2)
        self.confirm = tkinter.Button(configFrame, text="confirm", command=self.confirm)
        self.confirm.grid(row=2, column=2, sticky='w')

        '''
        mutation part
        '''
        # LabelFrame using tab1 as the parent
        mutationFrame=ttk.LabelFrame(mutationtab)
        mutationFrame.grid(column=0, row=0, padx=4, pady=8)
        operatorframe = ttk.LabelFrame(mutationFrame, text="mutation operator")
        operatorframe.grid(column=0, row=1, padx=4, pady=8)
        # chose the operators to do mutation
        self.operators = ''
        self.ops = ['ABV', 'AOR', 'BOR', 'CSR', 'LOR', 'LVR', 'MCD', 'ROR', 'ROV', 'SOR', 'STDC', 'UOI', 'UOR']
        self.opstatus = []
        for i in range(len(self.ops)):
            self.opstatus.append(tkinter.BooleanVar())
            tkinter.Checkbutton(operatorframe, text=self.ops[i], variable=self.opstatus[-1]).grid(row=1+i//4, column=(i)%4+1)
        konglabel3=tkinter.Label(operatorframe, text='  ')
        konglabel3.grid(row=0,column=0)
        konglabel3=tkinter.Label(operatorframe, text='  ')
        konglabel3.grid(row=6,column=4)
        # show mutants
        mutantFrame=ttk.LabelFrame(mutationFrame,text='mutant')
        mutantFrame.grid(column=1, row=1, padx=8, pady=4)
        mutationlabel = tkinter.Label(mutantFrame, text='  ')
        mutationlabel.grid(row=3, column=0)
        mutationlabel2 = tkinter.Label(mutantFrame, text='  ')
        mutationlabel2.grid(row=3, column=3)
        self.mutantnum = tkinter.StringVar()
        self.mutantnum.set('number: 0')
        numberlabel = tkinter.Label(mutantFrame, textvariable=self.mutantnum)
        numberlabel.grid(row=2, column=1)
        mutationscrollbar = tkinter.Scrollbar(mutantFrame)
        mutationscrollbar.grid(row=3, column=2, sticky='ns')
        self.mutationlistbox = tkinter.Listbox(mutantFrame, height=8,yscrollcommand=mutationscrollbar.set)
        self.mutationlistbox.bind('<Double-Button-1>', self.printmutant)
        self.mutationlistbox.grid(row=3, column=1)
        mutationscrollbar.config(command=self.mutationlistbox.yview)
        # show one mutant
        frame2 = ttk.LabelFrame(mutationFrame,text='mutant program')
        frame2.grid(column=0, row=0, padx=8, pady=4,columnspan=3)
        mutaantscrollbar = tkinter.Scrollbar(frame2)
        mutaantscrollbar.grid(row=4, column=2, sticky='ns')
        self.mutantlistbox = tkinter.Listbox(frame2, width=60, height=8,yscrollcommand=mutaantscrollbar.set)
        self.mutantlistbox.grid(row=4, column=1)
        mutaantscrollbar.config(command=self.mutantlistbox.yview)
        # the button of do mutation
        self.mutation = tkinter.Button(mutationFrame, text="generate", command=self.mutation)
        self.mutation.grid(row=1, column=2,sticky='e')

        '''
        combine part
        '''
        combineFrame = ttk.LabelFrame(combinetab)
        combineFrame.grid(column=0, row=0, padx=8, pady=4)
        # show block
        blockframe = ttk.LabelFrame(combineFrame,text='block')
        blockframe.grid(column=0, row=1, padx=8, pady=4)
        konglabel4=tkinter.Label(blockframe, text='  ')
        konglabel4.grid(row=1,column=0)
        konglabel4=tkinter.Label(blockframe, text='  ')
        konglabel4.grid(row=1,column=3)
        self.blocknum = tkinter.StringVar()
        self.blocknum.set('number: 0')
        blocknumberlabel = tkinter.Label(blockframe, textvariable=self.blocknum)
        blocknumberlabel.grid(row=1, column=1)
        blockscrollbar = tkinter.Scrollbar(blockframe)
        blockscrollbar.grid(row=2, column=2, sticky='ns')
        self.blocklistbox = tkinter.Listbox(blockframe, height=8,yscrollcommand=blockscrollbar.set)
        self.blocklistbox.bind('<Double-Button-1>', self.printblockmut)
        self.blocklistbox.grid(row=2, column=1)
        blockscrollbar.config(command=self.blocklistbox.yview)
        # show mutants in the block
        bmutantframe=ttk.LabelFrame(combineFrame,text='mutant in block')
        bmutantframe.grid(column=1, row=1, padx=8, pady=4)
        konglabel4=tkinter.Label(bmutantframe, text='  ')
        konglabel4.grid(row=1,column=0)
        konglabel4=tkinter.Label(bmutantframe, text='  ')
        konglabel4.grid(row=1,column=5)
        self.blockmutnum = tkinter.StringVar()
        self.blockmutnum.set('number: 0')
        blockmutnumberlabel = tkinter.Label(bmutantframe,textvariable=self.blockmutnum)
        blockmutnumberlabel.grid(row=1, column=3)
        blockmutscrollbar = tkinter.Scrollbar(bmutantframe)
        blockmutscrollbar.grid(row=2, column=4, sticky='ns')
        self.blockmutlistbox = tkinter.Listbox(bmutantframe,height=8,yscrollcommand=blockmutscrollbar.set)
        self.blockmutlistbox.grid(row=2, column=3)
        blockmutscrollbar.config(command=self.blockmutlistbox.yview)
        # show the block program file
        bprogramframe=ttk.LabelFrame(combineFrame,text='block program')
        bprogramframe.grid(row=0,column=0, padx=8, pady=4,columnspan=3)
        blockfilescrollbar = tkinter.Scrollbar(bprogramframe)
        blockfilescrollbar.grid(row=3, column=4, sticky='ns')
        self.blockfilelistbox = tkinter.Listbox(bprogramframe, width=60,height=8,yscrollcommand=blockfilescrollbar.set)
        self.blockfilelistbox.grid(row=3, column=1,sticky='we')
        blockfilescrollbar.config(command=self.blockfilelistbox.yview)
        # the button of do combine
        self.combine = tkinter.Button(combineFrame, text="synthesize", command=self.combinemt)
        self.combine.grid(row=1, column=2)

        '''
        test part
        '''
        testFrame = ttk.LabelFrame(testtab)
        testFrame.grid(column=0, row=0, padx=8, pady=4)
        # show unkilled block
        ukblockFrame=ttk.LabelFrame(testFrame,text='unkilled block')
        ukblockFrame.grid(column=0, row=0, padx=8, pady=4)
        self.unkilledblocknum = tkinter.StringVar()
        self.unkilledblocknum.set('number: 0')
        unkilledblocknumberlabel = tkinter.Label(ukblockFrame,textvariable=self.unkilledblocknum)
        unkilledblocknumberlabel.grid(row=6, column=1)
        unkilledblockscrollbar = tkinter.Scrollbar(ukblockFrame)
        unkilledblockscrollbar.grid(row=7, column=2, sticky='ns')
        self.unkilledblocklistbox = tkinter.Listbox(ukblockFrame,width=27,height=15,yscrollcommand=unkilledblockscrollbar.set)
        self.unkilledblocklistbox.bind('<Double-Button-1>', self.printunkilledblockmut)
        self.unkilledblocklistbox.grid(row=7, column=1)
        unkilledblockscrollbar.config(command=self.unkilledblocklistbox.yview)
        # show unkilled mutant in block
        ukmutantFrame=ttk.LabelFrame(testFrame,text='unkilled mutant in block ')
        ukmutantFrame.grid(column=1, row=0, padx=8, pady=4,columnspan=2)
        self.unkilledblockmutnum = tkinter.StringVar()
        self.unkilledblockmutnum.set('number: 0')
        unkilledblockmutnumberlabel = tkinter.Label(ukmutantFrame,textvariable=self.unkilledblockmutnum)
        unkilledblockmutnumberlabel.grid(row=6, column=3)
        unkilledblockmutscrollbar = tkinter.Scrollbar(ukmutantFrame)
        unkilledblockmutscrollbar.grid(row=7, column=4, sticky='ns')
        self.unkilledblockmutlistbox = tkinter.Listbox(ukmutantFrame,width=27,height=15,yscrollcommand=unkilledblockmutscrollbar.set)
        self.unkilledblockmutlistbox.grid(row=7, column=3)
        unkilledblockmutscrollbar.config(command=self.unkilledblockmutlistbox.yview)
        # test result
        resultFrame=ttk.LabelFrame(testFrame,text='test result')
        resultFrame.grid(column=0, row=1, padx=8, pady=4)
        self.allnum = tkinter.StringVar()
        self.allnum.set('all mutant:0')
        alllabel = tkinter.Label(resultFrame, textvariable=self.allnum)
        alllabel.grid(row=2, column=0,sticky='w')
        self.killednum = tkinter.StringVar()
        self.killednum.set('killed mutant:0')
        konglabel4=tkinter.Label(resultFrame, text='  ')
        konglabel4.grid(row=2,column=1)
        killedlabel = tkinter.Label(resultFrame, textvariable=self.killednum)
        killedlabel.grid(row=2, column=2,sticky='w')
        self.unkillednum = tkinter.StringVar()
        self.unkillednum.set('unkilled mutant:0')
        unkilledlabel = tkinter.Label(resultFrame, textvariable=self.unkillednum)
        unkilledlabel.grid(row=4, column=0,sticky='w')
        self.timeused = tkinter.StringVar()
        self.timeused.set('time used:0s')
        timelabel = tkinter.Label(resultFrame, textvariable=self.timeused)
        timelabel.grid(row=4, column=2,sticky='w')
        # the button of do test
        self.test = tkinter.Button(testFrame, text="test", command=self.testmt)
        self.test.grid(row=1, column=1)
        self.result = tkinter.Button(testFrame, text="view log", command=self.viewresult)
        self.result.grid(row=1, column=2,sticky='w')
        '''
        begin to run
        '''
        window.mainloop()

    def mutation(self):
        os.system('mkdir '+self.rootpathentry.get()+'/Mutant')
        shutil.rmtree(self.mutantspathentry.get())
        os.mkdir(self.mutantspathentry.get())
        self.operators = ''
        for i in range(len(self.ops)):
            if self.opstatus[i].get():
                self.operators += self.ops[i] + ' '
        self.adder.python_doMutation(bytes(self.sourcepathentry.get(), encoding='utf8'), \
                                     bytes(self.workpath + "Mutant/", encoding='utf8'), \
                                     bytes(self.operators, encoding='utf8'))
        for root, dirs, files in os.walk(self.mutantspathentry.get()):
            print(root)  # 当前目录路径
            print(dirs)  # 当前路径下所有子目录
            print(files)  # 当前路径下所有非目录子文件
            files.sort()
            self.mutantnum.set('number： ' + str(len(files)))
            for file in files:
                self.mutationlistbox.insert(tkinter.END, file)
            break
        if self.mutationlistbox.size() != 0:
            self.mutationlistbox.selection_set(0, 0)
            self.showmutant(self.mutationlistbox.get(self.mutationlistbox.curselection()))

    def printmutant(self, event):
        mutantname = self.mutationlistbox.get(self.mutationlistbox.curselection())
        #self.mutationlistbox.delete(0, tkinter.END)
        self.showmutant(mutantname)

    def showmutant(self, mutantname):
        self.mutantlistbox.delete(0, tkinter.END)
        try:
            loc = 0
            sourcefile = open(self.workpath + 'Source/' + self.sourcename + self.sourceextendname)
            print(self.workpath + 'Source/' + self.sourcename + self.sourceextendname,
                  self.mutantspathentry.get() + mutantname)

            with open(self.mutantspathentry.get() + mutantname) as mutfile:
                tmp1 = mutfile.readline()
                tmp2 = sourcefile.readline()
                while tmp1:
                    if tmp1 == tmp2:
                        self.mutantlistbox.insert(loc, tmp1)
                    else:
                        self.mutantlistbox.insert(loc, tmp1 + "/*mutant*/")
                        self.mutantlistbox.itemconfig(loc, fg='blue')
                        loc += 1
                        self.mutantlistbox.insert(loc, tmp2 + "/*source program*/")
                        self.mutantlistbox.itemconfig(loc, fg='red')
                    tmp1 = mutfile.readline()
                    tmp2 = sourcefile.readline()
                    loc += 1
            sourcefile.close()
        except OSError as reason:
            print('出错啦！' + str(reason))

    def combinemt(self):
        shutil.rmtree(self.workpath + 'Mutation/')
        os.mkdir(self.workpath + 'Mutation/')
        shutil.rmtree(self.workpath + 'Block/')
        os.mkdir(self.workpath + 'Block/')
        self.adder.python_doCombine()
        for root, dirs, files in os.walk(self.workpath + 'Mutation/'):
            # print(root)  # 当前目录路径
            # print(dirs)  # 当前路径下所有子目录
            # print(files)  # 当前路径下所有非目录子文件
            # dirs.sort()
            self.mutantnum.set('number： ' + str(len(files)))
            for file in dirs:
                self.blocklistbox.insert(tkinter.END, file)
            break
        if self.blocklistbox.size() != 0:
            self.blocklistbox.selection_set(0, 0)
            self.showblockmut(self.blocklistbox.get(self.blocklistbox.curselection()))
        self.blocknum.set('number: '+str(self.blocklistbox.size()))

    def printblockmut(self, event):
        #self.blocklistbox.delete(0, tkinter.END)
        blockname = self.blocklistbox.get(self.blocklistbox.curselection())
        self.showblockmut(blockname)

    def showblockmut(self, blockname):
        self.blockmutlistbox.delete(0,tkinter.END)
        print(self.workpath + 'Mutation/' + blockname)
        for root, dirs, files in os.walk(self.workpath + 'Mutation/' + blockname):
            # print(root)  # 当前目录路径
            # print(dirs)  # 当前路径下所有子目录
            # print(files)  # 当前路径下所有非目录子文件
            # files.sort()
            for file in files:
                self.blockmutlistbox.insert(tkinter.END, file)
            self.blockmutnum.set('number: ' + str(self.blockmutlistbox.size()))
            break
        # show block file
        self.blockfilelistbox.delete(0,tkinter.END)
        try:
            with open(self.workpath + 'Block/' + blockname + self.sourceextendname) as blockfile:
                tmp = blockfile.readline()
                while tmp:
                    self.blockfilelistbox.insert(tkinter.END, tmp)
                    tmp = blockfile.readline()
        except OSError as reason:
            print('出错啦！' + str(reason))

    def testmt(self):
        os.system("pwd")
        os.system("bash "+self.workpath+"Scripts/TimetheRun.sh")
        for root, dirs, files in os.walk(self.workpath+'Scripts/'):
            for dir in dirs:
                for i, j, k in os.walk(dir):
                    if len(k)!=0:
                        self.unkilledblockmutlistbox.insert(tkinter.END, dir)
                    break
            break
        if self.unkilledblocklistbox.size()!=0:
            self.unkilledblocklistbox.selection_set(0, 0)
            self.showunkilledblockmut(self.unkilledblocklistbox.get(self.unkilledblocklistbox.curselection()))
        self.unkilledblocknum.set('unkilled block:'+str(self.unkilledblocklistbox.size()))
        allmutnum=0
        for root, dirs, files in os.walk(self.workpath+'Mutation/'):
            for dir in dirs:
                for i, j, k in os.walk(self.workpath+'Mutation/'+dir):
                    allmutnum+=len(k)
                    break
            break
        self.allnum.set('number: '+str(allmutnum))
        self.killednum.set('killed mutant:'+str(allmutnum-self.unkilledblocklistbox.size()))
        with open(self.workpath+'Scripts/'+'output2.txt','r') as outputfile:
            self.timeused.set('time used:'+outputfile.readlines()[-3].split()[1])

    def viewresult(self):
        os.system('gedit '+self.rulepath+'/../../Scripts/output.txt')


    def printunkilledblockmut(self):
        blockname = self.unkilledblocklistbox.get(self.unkilledblocklistbox.curselection())
        self.showunkilledblockmut(blockname)

    def showunkilledblockmut(self,blockname):
        self.unkilledblockmutlistbox.delete((0,tkinter.END))
        for root, dirs, files in os.walk(self.workpath+'Scripts/'+blockname):
            print(root)  # 当前目录路径
            print(dirs)  # 当前路径下所有子目录
            print(files)  # 当前路径下所有非目录子文件
            files.sort()
            for file in files:
                self.unkilledblockmutlistbox.insert(tkinter.END,file)
            break
        self.unkilledblockmutnum.set('number: '+str(self.unkilledblockmutlistbox.size()))

    def imports(self):
        filename = filedialog.askopenfilename(title='select config file', filetypes=[('txt', '*.json')])
        with open(filename, 'r') as load_f:
            load_dict = json.load(load_f)
            print(load_dict)
            self.rootpathentry.delete(0, tkinter.END)
            self.rootpathentry.insert(0, load_dict["rootpath"])
            self.sourcepathentry.delete(0, tkinter.END)
            self.sourcepathentry.insert(0, load_dict["sourcepath"])
            self.sourcename, self.sourceextendname = os.path.splitext(os.path.split(load_dict["sourcepath"])[1])

            self.workpath = load_dict["rootpath"] + "Pecma/" + self.sourcename + "/"
            if load_dict.get("mutantspath"):
                self.mutantspathentry.delete(0, tkinter.END)
                self.mutantspathentry.insert(0, load_dict["mutantspath"])
            else:
                self.mutantspathentry.insert(0, self.workpath + "Mutant/")
            self.testcasepathentry.delete(0, tkinter.END)
            self.testcasepathentry.insert(0, load_dict["testcasepath"])
            self.rulepath = self.workpath + "Source/blockrule.txt"
            self.blockcontextpath = self.workpath + "Source/blockcontext.txt"
            self.subBlockpath = self.workpath + "Source/subBlock.txt"

    def confirm(self):
        if (not os.path.exists(self.workpath)):
            os.mkdir(self.workpath)
        if (os.path.exists(self.workpath + "pecma.config")):
            os.remove(self.workpath + "pecma.config")
        # with open(self.workpath + "pecma.config", 'w') as config_f:
        #     config_f.writelines("#PECMA CONFIG FILE")
        #     config_f.writelines("\n")
        #     config_f.writelines("rootpath=" + self.rootpathentry.get())
        #     config_f.writelines("\n")
        #     config_f.writelines("sourcepath=" + self.sourcepathentry.get())
        #     config_f.writelines("\n")
        #     config_f.writelines("mutantspath=" + self.mutantspathentry.get())
        #     config_f.writelines("\n")
        #     config_f.writelines("rulepath=" + self.rulepath)
        #     config_f.writelines("\n")
        #     config_f.writelines("blockcontextpath=" + self.blockcontextpath)
        #     config_f.writelines("\n")
        #     config_f.writelines("subBlockpath=" + self.subBlockpath)
        #     config_f.writelines("\n")
        #     config_f.writelines("testcasepath=" + self.testcasepathentry.get())
        #     config_f.writelines("\n")
        self.adder.python_preprocess(bytes("pecma.config", encoding='utf8'))
        # # print(self.sourcepathentry.get(),self.rootpathentry.get()+self.sourcename+"Source/")
        self.adder.python_doBlock(bytes(self.sourcepathentry.get(), encoding='utf8'), \
                                  bytes(self.workpath + "Source/", encoding='utf8'))
        with open(self.workpath + "Source/" + "blockrule.txt") as rulefile:
            tmp = rulefile.readline()
            while tmp:
                self.rulelistbox.insert(tkinter.END, tmp)
                tmp = rulefile.readline()

    def selectfolder(self, path):
        foldername = filedialog.askdirectory(title='select folder')
        path.delete(0, tkinter.END)
        path.insert(0, foldername)

    def selectfile(self, path):
        filename = filedialog.askopenfilename(title='select file')
        path.delete(0, tkinter.END)
        path.insert(0, filename)


Cpms()
