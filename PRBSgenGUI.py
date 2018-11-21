# Graphical User Interface
# Pseudo Random Binary Sequence generator
# Frequency - 10kHz,.....,100kHz
# Binary bits - 4...14
# Duration - As long Arduino Nano can take in long variable, limited by delayMicroseconds (70 minute rollover)

import time
import serial
from tkinter import *


arduinoData = serial.Serial('COM7', baudrate=1000000)  # Initialise....

global tempDuration

def CurrentFrequency(frequency):
    freqLookup = {'10 kHz': 10000, '15 kHz': 15000, '20 kHz': 20000, '25 kHz': 25000,
                  '30 kHz': 30000, '35 kHz': 35000, '40 kHz': 40000, '45 kHz': 45000,
                  '50 kHz': 50000, '55 kHz': 55000, '60 kHz': 60000, '65 kHz': 65000,
                  '70 kHz': 70000, '75 kHz': 75000, '80 kHz': 80000, '85 kHz': 85000,
                  '90 kHz': 90000, '95 kHz': 95000, '100 kHz': 100000}
    freq = freqLookup[frequency]
    return str(freq)


def CurrentPRBS(prbs):
    prbsLookup = {'4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9, '10': 10, '11': 11,
    			 '12': 12, '13': 13, '14': 14}
    prbs = prbsLookup[prbs]
    return str(prbs)


def CurrentPeriod(period):
    periodLookup = {'1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, 
    				'9': 9, '10': 10}
    period = periodLookup[period] + periodOffset
    return str(period)

	
def tempDur():
    global tempDuration
    X = tempDuration
    return X


class DemoClass:

		def __init__(self):
        self.window = Tk()
        self.window.title("PRBS Generator")
        self.window.geometry("400x275+500+50")
        self.frame = Frame(self.window)
        self.frame.grid(column=0, row=0, sticky=(N, W, E, S))
        self.frame.columnconfigure(0, weight=1)
        self.frame.rowconfigure(0, weight=1)
        self.frame.pack(pady=25, padx=25)

# =============================================================
        self.runB = Button(self.frame, activeforeground="green", text="Run Test",
        					command=self.runTest)  #
        self.runB.grid(row=7, column=3)  
		
        self.stopB = Button(self.frame, activeforeground="red", text="Stop Test",
        					command=self.stopTest)  #
        self.stopB.grid(row=7, column=1) 

        self.label1 = Label(self.frame, text="PRBS bits")  #
        self.label1.grid(row=2, column=1)

        self.label2 = Label(self.frame, text="Frequency")  #
        self.label2.grid(row=2, column=2)

        self.label3 = Label(self.frame, text="Duration (ms)")  #
        self.label3.grid(row=2, column=3)

        self.durButton = Button(self.frame, text='Set', command=self.processDuration)
        self.durButton.grid(row=4, column=3)

        self.CurrentSetDuration = Label(self.frame, text=" ")  #
        self.CurrentSetDuration.grid(row=5, column=1, columnspan=3)

        self.labelY = Label(self.frame, text=" ")  #
        self.labelY.grid(row=6, column=1)

# =============================================================

        # self.window.after(1000, self.timedFunction)
        # self.window.mainloop()

# =============================================================
        self.freqVar = StringVar(self.frame)
        self.prbsVar = StringVar(self.frame)
        self.periodsVar = StringVar(self.frame)

        # Frequency options
        frequencyChoices = ('10 kHz', '15 kHz', '20 kHz', '25 kHz',
                            '30 kHz', '35 kHz', '40 kHz', '45 kHz',
                            '50 kHz', '55 kHz', '60 kHz', '65 kHz',
                            '70 kHz', '75 kHz', '80 kHz', '85 kHz',
                            '90 kHz', '95 kHz', '100 kHz')
        self.freqVar.set('10 kHz')  # set the default option

        # PRBS bit options
        PRBSchoices = ('4', '5', '6', '7', '8', '9', '10', '11', '12', '13',
        			  '14')
        self.prbsVar.set('4')

        periodChoices = ('1', '2', '3', '4', '5', '6', '7', '8', '9', '10')
        self.periodsVar.set('1')

        popupMenu1 = OptionMenu(self.frame, self.prbsVar, *PRBSchoices)
        popupMenu1.grid(row=3, column=1)

        popupMenu2 = OptionMenu(self.frame, self.freqVar, *frequencyChoices)
        popupMenu2.grid(row=3, column=2)

        self.EntryField1 = Entry(self.frame, width=8)
        self.EntryField1.grid(row=3, column=3)
# =============================================================

        tempDur = self.EntryField1.get()
        durButton = Button(self.frame, text='Set', command=self.processDuration)
        durButton.grid(row=4, column=3)
# =============================================================

        self.DateLabel = Label(self.frame, text="")
        self.DateLabel.grid(row=0, column=1, columnspan=3)
        self.ClockLabel = Label(self.frame, text="")
        self.ClockLabel.grid(row=1, column=1, columnspan=3)

        self.update_clock()
        self.window.mainloop()

# =====================Clock===================================

    def update_clock(self):
        now = time.strftime("%H:%M:%S")
        date = time.strftime("%A %d %B %Y")
        self.ClockLabel.configure(text="Time: " + now)
        self.DateLabel.configure(text="Date: " + date)
        self.window.after(1000, self.update_clock)

    # =============================================================
    def processDuration(self):
        print('Testing Duration Entry')
        global tempDuration
        tempDuration = self.EntryField1.get()
        self.CurrentSetDuration.configure(text="Set Duration is: " + tempDuration + " ms")
        print('Temp Duration = ', tempDuration)
		

    def stopTest(self):
        stopTest = 'x'
        arduinoData.write(stopTest.encode())
        print('Stop Test', stopTest)

    def runTest(self):
        comma = str(',')
        beginTrans = 'y'  # Indicates that a test is about commence, for Arduino
        endTrans = 'z'  # Indicates end of transmission
        sampFreq = CurrentFrequency(self.freqVar.get())  # Sampling Frequency
        prbsL = CurrentPRBS(self.prbsVar.get())  # PRBS length
        Duration = tempDur()

        transmitData = beginTrans + prbsL + comma + sampFreq + comma + Duration + endTrans
        arduinoData.write(transmitData.encode())
        print(transmitData)

    def timedFunction(self):
		#Debugging function
        print("I'm in a function that keeps rescheduling itself to be executed after 1 second!")
        self.window.after(1000, self.timedFunction)

instance = DemoClass()
