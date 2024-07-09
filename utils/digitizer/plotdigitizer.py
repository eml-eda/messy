# Plot Digitizer
# Import an image, and it will help you digitize the data

from PyQt5 import QtCore # QObject, pyqtSignal
from PyQt5 import QtWidgets # Widgets, windows, QApplication
from PyQt5 import uic # .ui file interpreting
import sys
import os
import csv
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.widgets import TextBox
import numpy as np

mpl.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure
from matplotlib.axes import Axes

import utils
from utils import Coordinate,Transformation,Dataset,DatasetParams

# HELPER FUNCTIONS
def find_lines(ax,label):
    lines = ax.get_lines() # All lines plotted; includes axis/frame lines
    for l in lines:
        if l.get_label() == label:
            return l
    return None

class MplCanvas(FigureCanvasQTAgg):
    def __init__(self,parent=None,width=5,height=4,dpi=100):
        self.fig = Figure(figsize=(width,height),dpi=dpi)
        self.fig.set_frameon(False)
        
        self.ax_main = self.fig.add_axes([0.1,0.1,0.85,0.85])
        self.ax_main.set_frame_on(True)
        self.ax_main.set_xticks([])
        self.ax_main.set_yticks([])

        self.inset_size = 0.3
        self.ax_inset = self.fig.add_axes([0.65,0.65,self.inset_size,self.inset_size])
        self.ax_inset.axis('off')
        self.ax_inset.set_xlim([0,60])
        self.ax_inset.set_ylim([60,0])


        super(MplCanvas, self).__init__(self.fig)

class ImageFile:
    def __init__(self,fname=None):
        if fname:
            self.fname = fname
            self.imgdata = mpimg.imread(fname)
            self.extension = fname[fname.rfind('.') : ] # e.g. '.png'
            self.width_px = self.imgdata.shape[0]
            self.height_px = self.imgdata.shape[1]
            self.depth = self.imgdata.shape[2]
            
        else:
            self.fname = None
            self.imgdata = None
    def plot_image(self,ax):
        if len(self.imgdata) > 0:
            ax.imshow(self.imgdata)
        else:
            print("Warning: No image data available.")
    def load_image_data(self,fname):
        self.fname = fname
        self.imgdata = mpimg.imread(fname)
        self.extension = fname[fname.rfind('.') : ] # e.g. '.png'
        self.width_px = self.imgdata.shape[0]
        self.height_px = self.imgdata.shape[1]
        self.depth = self.imgdata.shape[2]

class ApplicationState:
    def __init__(self):
        self.mode = "select_axes" # select_axes, select_datapoint,  or edit_datapoints
        self.active_dataset = "default"
        self.workingdir = os.getcwd()
        self.hide_browse_warning = False

# ImageAxis class
# Container for coordinates, represents one axis for the image axes
# Has both matplotlib (mpl) and image (img) points
class ImageAxis:
    def __init__(self, label, logscale=False):
        self.label = label
        self.logscale = logscale
        self.p1_mpl = Coordinate(0,0) # matplotlib coordinates
        self.p2_mpl = Coordinate(0,0)
        self.p1_img = Coordinate(0,0) # image axes coordinates
        self.p2_img = Coordinate(0,0)

# ImageAxes class
# Stores two ImageAxis objects and calculates a transformation from mpl coordinates to img coordinates
# Includes some math utils when you need them
class ImageAxes:
    def __init__(self, label,logscalex=False, logscaley=False):
        self.label = label
        self.xaxis = ImageAxis('xaxis',logscale=logscalex)
        self.yaxis = ImageAxis('yaxis',logscale=logscaley)

        self.xaxis.p1_mpl = Coordinate(0,0) # Default axes, in mpl coordinates
        self.xaxis.p2_mpl = Coordinate(1,0)
        self.yaxis.p1_mpl = Coordinate(0,0)
        self.yaxis.p2_mpl = Coordinate(0,1)

        # Transformation for mpl -> image
        self.transformation = Transformation() # Default is identity matrix

        self.active_point = "xaxis1" # "xaxis1","xaxis2","yaxis1"

    def transform_to_img(self,p1_mpl):
        point = self.transformation.transform(p1_mpl)
        # This is where logscale conversion takes place
        if self.xaxis.logscale:
            point.x = np.power(10,point.x)
        if self.yaxis.logscale:
            point.y = np.power(10,point.y)
        return point
    def transform_to_mpl(self,p1_img):
        # TODO This doesn't work yet with log scale
        point = self.transformation.invert_transform(p1_img)
        return point
    def transform_dataset_to_img(self,ds):
        dsout = Dataset(DatasetParams("_output_export"),points=[])
        for p in range(len(ds.points)):
            pT = self.transform_to_img(ds.points[p])
            dsout.append_point(pT)
        return dsout

    # MATH UTILS
    def get_xslope(self):
        p1 = self.xaxis.p1_mpl
        p2 = self.xaxis.p2_mpl
        if p1.x - p2.x != 0:
            return ( p1.y-p2.y )/( p1.x-p2.x )
        else:
            return np.Inf
    def get_yslope(self):
        p1 = self.yaxis.p1_mpl
        p2 = self.yaxis.p2_mpl
        if p1.x - p2.x != 0:
            return ( p1.y-p2.y )/( p1.x-p2.x )
        else:
            return np.Inf
    def get_xaxis_yint(self):
        slope = self.get_xslope()
        p1 = self.xaxis.p1_mpl
        p2 = self.xaxis.p2_mpl
        if slope != np.Inf:
            return p1.y - p1.x*slope
        else:
            return np.NaN
    def get_yaxis_yint(self):
        slope = self.get_yslope()
        p1 = self.yaxis.p1_mpl
        p2 = self.yaxis.p2_mpl
        if slope != np.Inf:
            return p1.y - p1.x*slope
        else:
            return np.NaN
    # Return angle (in deg) from x to y (mpl coordinates)
    def get_skew_angle(self):
        # Find the angle between slopes
        slope1 = self.get_xslope()
        slope2 = self.get_yslope()
        if slope1 != np.Inf and slope2 != np.Inf:
            return np.arctan2(np.abs(slope2-slope1),np.abs(1+slope2*slope1))*180/np.pi
        else:
            return 90
    # Get intersection of axes (mpl coordinates)
    def get_axes_intersection(self):
        slope1 = self.get_xslope()
        slope2 = self.get_yslope()
        intercept1 = self.get_xaxis_yint()
        intercept2 = self.get_yaxis_yint()

        if slope1 != np.Inf and slope2 != np.Inf and intercept1 != np.NaN and intercept2 != np.NaN:
            x = (intercept2-intercept1)/(slope1-slope2)
            y = slope1*x+intercept1
            return ( x,y )
        else:
            return ( 0,0 )

    # PLOTTING UTILS
    def get_xdata(self):
        return [self.xaxis.p1_mpl.x, self.xaxis.p2_mpl.x, self.yaxis.p1_mpl.x, self.yaxis.p2_mpl.x]
    def get_ydata(self):
        return [self.xaxis.p1_mpl.y, self.xaxis.p2_mpl.y, self.yaxis.p1_mpl.y, self.yaxis.p2_mpl.y]
    def plot_axes(self,ax,canvas,markersize=None):
        # Check if this dataset (with this label) has been added to the plot yet
        l = find_lines(ax,self.label)
        if l == None:
            # Add this dataset to the plot
            lines = ax.plot(self.get_xdata(),self.get_ydata())
            l = lines[0]
            l.set_linestyle('')
            l.set_marker('.')
            if markersize==None:
                l.set_markersize(8)
            else:
                l.set_markersize(markersize) # Override
            l.set_markeredgewidth(0)
            l.set_markerfacecolor('b')
            l.set_label(self.label) # So we can find this again
        else:
            # Set the data to the current data
            l.set_data(self.get_xdata(),self.get_ydata())
            if markersize == None:
                l.set_markersize(8)
            else:
                l.set_markersize(markersize) # Override
        canvas.draw()

class EditDatasetDialog(QtWidgets.QDialog):
    def __init__(self):
        super(EditDatasetDialog, self).__init__()
        uic.loadUi("edit_dataset_dialog.ui",self)

        # Callbacks
        self.markercolor_button.clicked.connect(self.color_cb)

    def open_dlg(self,dsparams):
        # dsparams are DatasetParams. New params are returned if user selects OK.
        self.datasetlabel_tb.setText(dsparams.label)
        self.datasetmarker_combo.setCurrentText(dsparams.marker)
        self._set_stylesheet_color(dsparams.markercolor)
        conf = self.exec_()
        if conf:
            label = self.datasetlabel_tb.text()
            marker = self.datasetmarker_combo.currentText()
            markercolor = self._get_color_from_stylesheet()
            new_params = DatasetParams(label=label,marker=marker,markercolor=markercolor)
            return new_params
        else:
            return dsparams

    def color_cb(self):
        color = QtWidgets.QColorDialog.getColor()

        if color.isValid():
            rgbf = color.getRgbF()
            self._set_stylesheet_color(rgbf)
            self.markercolor = rgbf

    def _get_color_from_stylesheet(self):
        ss_str = self.markercolor_button.styleSheet()
        idx1 = ss_str.find('(')+1
        idx2 = ss_str.find(',')
        idx3 = ss_str.find(',',idx2+1)
        idx4 = ss_str.find(')')
        
        r = float(ss_str[idx1:idx2])/255
        g = float(ss_str[idx2+1:idx3])/255
        b = float(ss_str[idx3+1:idx4])/255
        return [r,g,b]

    def _set_stylesheet_color(self,rgbf):
        color_rgb255 = [int(c*255) for c in rgbf]
        ss_str = "background-color: rgb(%d,%d,%d);" % (color_rgb255[0], color_rgb255[1], color_rgb255[2])
        self.markercolor_button.setStyleSheet(ss_str)
        




class MainApplication:
    def __init__(self):
        self.app = QtWidgets.QApplication(sys.argv)
        self.win = uic.loadUi("mainwindow.ui")

        # QT sizing policies
        QExpanding = QtWidgets.QSizePolicy.Policy.Expanding
        expand_policy = QtWidgets.QSizePolicy(QExpanding,QExpanding)
        QPreferred = QtWidgets.QSizePolicy.Policy.Preferred
        preferred_policy = QtWidgets.QSizePolicy(QPreferred,QPreferred)

        # Adding matplotlib canvas widget
        self.plotwdg = MplCanvas()
        self.win.plotspace.addWidget(self.plotwdg)
        self.plotwdg.setSizePolicy(expand_policy)
        self.plotwdg.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.plotwdg.setFocus()

#        self.image_data = ImageFile('screenshot_22Jul2020.png')
        self.image_data = ImageFile('plot1.png')
        self.image_data.plot_image(self.plotwdg.ax_main)
        self.image_data.plot_image(self.plotwdg.ax_inset)

        # Initialize application state and data
        self.state = ApplicationState()
        self.state.mode = "select_axes"
        self.datasets = np.array([Dataset(DatasetParams("default",markercolor=[0.0,1.0,0.0]))])
        self.state.active_dataset = self.datasets[0].label
        self.win.dataset_combo.insertItem(0,self.datasets[0].label)
        self.win.dataset_combo.setCurrentText(self.datasets[0].label)
        self.imgaxes = ImageAxes("axes1")

        # Update the plot of the image axes
        self.imgaxes.plot_axes(self.plotwdg.ax_main,self.plotwdg.fig.canvas)
        self.imgaxes.plot_axes(self.plotwdg.ax_inset,self.plotwdg.fig.canvas,markersize=16)

        # Initialize a text box for capturing axis positions
        self.label_tb = QtWidgets.QLabel("Axis coordinate: x,y",self.win)
        self.label_tb.setSizePolicy(preferred_policy)
        self.label_tb.setFixedSize(QtCore.QSize(200,30))
        self.label_tb.move(500,470)
        self.label_tb.setStyleSheet('background-color: rgb(255, 255, 255); font: 11pt "DejaVu Sans"');
        self.label_tb.setVisible(False)

        self.textbox = QtWidgets.QLineEdit(self.win)
        self.textbox.setSizePolicy(preferred_policy)
        self.textbox.setFixedSize(QtCore.QSize(200,30))
        self.textbox.move(500,500) # This will be overridden when it actually becoming visible
        self.textbox.setVisible(False)


        # Callbacks
        self.plotwdg.fig.canvas.mpl_connect('motion_notify_event',self.motion_cb)
        self.plotwdg.fig.canvas.mpl_connect('button_press_event',self.click_cb)
        self.plotwdg.fig.canvas.mpl_connect('key_press_event',self.keypress_cb)
        self.textbox.returnPressed.connect(self.textbox_finished_cb)
        self.win.exportdatabutton.pressed.connect(self.export_data_cb)
        self.win.changeaxbutton.pressed.connect(self.change_ax_cb)
        self.win.editdatasetbutton.pressed.connect(self.edit_dataset_cb)
        self.win.deletedatasetbutton.pressed.connect(self.delete_dataset_cb)
        self.win.newdatasetbutton.pressed.connect(self.new_dataset_cb)
        self.win.dataset_combo.currentTextChanged.connect(self.select_dataset_cb)
        self.win.logxcheck.stateChanged.connect(self.logx_change_cb)
        self.win.logycheck.stateChanged.connect(self.logy_change_cb)
        self.win.browse_button.pressed.connect(self.browse_image_cb)
        self.win.loadcsv_button.pressed.connect(self.browse_csvdata_cb)
        self.win.cleardata_button.pressed.connect(self.clear_datapoints_cb)
        self.win.editdata_button.pressed.connect(self.edit_datapoints_cb)



    def show(self):
        self.win.show()

    def update_inset(self, x, y):
        fig = self.plotwdg.fig
        canvas = fig.canvas
        ax_main = self.plotwdg.ax_main
        ax_inset = self.plotwdg.ax_inset
        inset_size = self.plotwdg.inset_size

        if x != None and y != None:
            xmax = fig.get_figwidth()*fig.get_dpi()
            ymax = fig.get_figheight()*fig.get_dpi()

            # Inset size is in terms of pct of the figure, so convert that to px:
            fig_to_disp = fig.transFigure
            disp_to_data = ax_main.transData.inverted()

            [x,y] = disp_to_data.transform(fig_to_disp.transform([x/xmax,y/ymax]))

            box_size = 60

            ax_inset.set_xlim([x-box_size, x+box_size])
            ax_inset.set_ylim([y+box_size, y-box_size])
        

    def click_cb(self,event):
        if self.state.mode == "select_axes":
            self.select_axes(event)
        elif self.state.mode == "select_datapoint":
            self.select_datapoint(event)
        elif self.state.mode == "edit_datapoints":
            self.move_datapoint(event)

    def edit_datapoints(self,event):
        pass

    def select_axes(self,event):
        if self.textbox.isVisible():
            # We're currently waiting for the user to finish entering a coordinate
            return
        if event.button == 1:
            if self.imgaxes.active_point == "xaxis1":
                self.imgaxes.xaxis.p1_mpl = Coordinate(event.xdata,event.ydata)
                self.get_text_popup(event)
            elif self.imgaxes.active_point == "xaxis2":
                self.imgaxes.xaxis.p2_mpl = Coordinate(event.xdata,event.ydata)
                self.get_text_popup(event)
            elif self.imgaxes.active_point == "yaxis1":
                self.imgaxes.yaxis.p1_mpl = Coordinate(event.xdata,event.ydata)
                self.state.mode = "select_datapoint" # Change mode
                self.get_text_popup(event)
            self.imgaxes.plot_axes(self.plotwdg.ax_main,self.plotwdg.fig.canvas)
            self.imgaxes.plot_axes(self.plotwdg.ax_inset,self.plotwdg.fig.canvas,markersize=16)
        elif event.button == 3: # Right click to undo
            if self.imgaxes.active_point == "xaxis2":
                self.imgaxes.xaxis.p1_mpl = Coordinate(0,0)
                self.imgaxes.active_point = "xaxis1"
            elif self.imgaxes.active_point == "yaxis1":
                self.imgaxes.xaxis.p2_mpl = Coordinate(0,0)
                self.imgaxes.active_point = "xaxis2"
            self.imgaxes.plot_axes(self.plotwdg.ax_main,self.plotwdg.fig.canvas)
            self.imgaxes.plot_axes(self.plotwdg.ax_inset,self.plotwdg.fig.canvas,markersize=16)
            
    # Textbox pop up for when the user selects an axes coordinate
    def get_text_popup(self,event):
        ax = self.plotwdg.ax_inset
        fig = self.plotwdg.fig

        # Get figure size
        figwidth = fig.get_figwidth()*fig.get_dpi()
        figheight = fig.get_figheight()*fig.get_dpi()

        # Get position of figure in window
        figx = self.plotwdg.pos().x()
        figy = self.plotwdg.pos().y()
        self.textbox.move(int(figx + event.x), int(figy + figheight - event.y))
        self.label_tb.move(int(figx + event.x), int(figy + figheight - event.y - 30))
        self.textbox.setVisible(True)
        self.label_tb.setVisible(True)

    # Select datapoint mode handler
    def select_datapoint(self,event):
        active_dataset = self.find_dataset(self.state.active_dataset)
        if event.button == 1:
            # Left mouse, select point
            active_dataset.append_point(Coordinate(event.xdata,event.ydata))
            active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
            active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                    self.plotwdg.fig.canvas,markersize=16)
        elif event.button == 3:
            if len(active_dataset) == 0:
                self.state.mode = 'select_axes'
                self.imgaxes.yaxis.p1_mpl = Coordinate(0,0)
                self.imgaxes.active_point = "yaxis1"
                self.imgaxes.plot_axes(self.plotwdg.ax_main,self.plotwdg.fig.canvas)
                self.imgaxes.plot_axes(self.plotwdg.ax_inset,self.plotwdg.fig.canvas,markersize=16)
            else:
                active_dataset.undo_append_point()
                active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
                active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                        self.plotwdg.fig.canvas,markersize=16)

    def find_dataset(self,label):
        for ds in self.datasets:
            if ds.label == label:
                return ds
        return None
    
    def get_active_dataset(self):
        return self.find_dataset(self.state.active_dataset)

    def keypress_cb(self,event):
        if event.key == 'left' or event.key == 'right' or event.key == 'up' or event.key == 'down':
            self.arrow_key_cb(event)

    # Callback for left, right, up, down keys
    def arrow_key_cb(self,event):
        if self.state.mode == "select_axes":
            self.nudge_axes_coordinates(event.key)
        elif self.state.mode == "select_datapoint":
            if len( self.find_dataset(self.state.active_dataset).points) == 0:
                self.nudge_axes_coordinates(event.key) # Nudge last coordinate
            else:
                active_dataset = self.find_dataset(self.state.active_dataset)
                active_dataset.nudge_last_point(event.key)

                active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
                active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                        self.plotwdg.fig.canvas,markersize=16)

    def nudge_axes_coordinates(self,direction):
        # These are all offset by 1, so go to the previous state
        if self.imgaxes.active_point == "xaxis1":
            p = self.imgaxes.yaxis.p1_mpl
        elif self.imgaxes.active_point == "xaxis2":
            p = self.imgaxes.xaxis.p1_mpl
        elif self.imgaxes.active_point == "yaxis1":
            p = self.imgaxes.xaxis.p2_mpl

        if direction=="up":
            p = Coordinate(p.x,p.y-1)
        elif direction=="down":
            p = Coordinate(p.x,p.y+1)
        elif direction=="left":
            p = Coordinate(p.x-1,p.y)
        elif direction=="right":
            p = Coordinate(p.x+1,p.y)
        else:
            print("Error: Unknown nudge direction")

        if self.imgaxes.active_point == "xaxis1":
            self.imgaxes.yaxis.p1_mpl = p
            # Need to recalculate matrix
            p1 = self.imgaxes.xaxis.p1_mpl.get_point_homog_vector()
            p2 = self.imgaxes.xaxis.p2_mpl.get_point_homog_vector()
            p3 = self.imgaxes.yaxis.p1_mpl.get_point_homog_vector()

            # These are transformed coordinates. If logscale, take log of entered value. 
            logx = self.imgaxes.xaxis.logscale
            logy = self.imgaxes.yaxis.logscale

            p1T = self.imgaxes.xaxis.p1_img.get_point_homog_vector()
            p2T = self.imgaxes.xaxis.p2_img.get_point_homog_vector()
            p3T = self.imgaxes.yaxis.p1_img.get_point_homog_vector()
            
            if logx:
                p1T[0][0] = np.log10(p1T[0][0])
                p2T[0][0] = np.log10(p2T[0][0])
                p3T[0][0] = np.log10(p3T[0][0])

            if logy:
                p1T[1][0] = np.log10(p1T[1][0])
                p2T[1][0] = np.log10(p2T[1][0])
                p3T[1][0] = np.log10(p3T[1][0])

            # Now use first 3 points to calculate transformation matrix
            tmat = utils.solve_transformation(p1,p2,p3,p1T,p2T,p3T)
            self.imgaxes.transformation = Transformation(matrix=tmat)
        elif self.imgaxes.active_point == "xaxis2":
            self.imgaxes.xaxis.p1_mpl = p
        elif self.imgaxes.active_point == "yaxis1":
            self.imgaxes.xaxis.p2_mpl= p
        self.imgaxes.plot_axes(self.plotwdg.ax_main,self.plotwdg.fig.canvas)
        self.imgaxes.plot_axes(self.plotwdg.ax_inset,self.plotwdg.fig.canvas,markersize=16)
    
    # For coordinate select mode, called after textbox entry is finished (enter is pressed)
    def textbox_finished_cb(self):
        if self.textbox.text() == "":
            return
        else:
            # This data point is the image axes point
            pvec = self.textbox.text().split(',')
            p1 = Coordinate(float(pvec[0].strip()),float(pvec[1].strip()))
            if self.imgaxes.active_point == "xaxis1":
                self.imgaxes.xaxis.p1_img = p1
                self.imgaxes.active_point = "xaxis2"
            elif self.imgaxes.active_point == "xaxis2":
                self.imgaxes.xaxis.p2_img = p1
                self.imgaxes.active_point = "yaxis1"
            elif self.imgaxes.active_point == "yaxis1":
                self.imgaxes.yaxis.p1_img = p1
                self.imgaxes.active_point = "xaxis1"
                
                # Now we have the points, we need to check which (if any) axis is log scale
                logx = self.imgaxes.xaxis.logscale
                logy = self.imgaxes.yaxis.logscale

                # Now get the points. Actually, only 3 points are required for a 2D affine transformation,
                # but getting 4 (two on each axis) is easier for a user to understand
                p1 = self.imgaxes.xaxis.p1_mpl.get_point_homog_vector()
                p2 = self.imgaxes.xaxis.p2_mpl.get_point_homog_vector()
                p3 = self.imgaxes.yaxis.p1_mpl.get_point_homog_vector()

                # These are transformed coordinates. If logscale, take log of entered value. 
                p1T = self.imgaxes.xaxis.p1_img.get_point_homog_vector()
                p2T = self.imgaxes.xaxis.p2_img.get_point_homog_vector()
                p3T = self.imgaxes.yaxis.p1_img.get_point_homog_vector()
                
                if logx:
                    p1T[0][0] = np.log10(p1T[0][0])
                    p2T[0][0] = np.log10(p2T[0][0])
                    p3T[0][0] = np.log10(p3T[0][0])

                if logy:
                    p1T[1][0] = np.log10(p1T[1][0])
                    p2T[1][0] = np.log10(p2T[1][0])
                    p3T[1][0] = np.log10(p3T[1][0])

                # Now use first 3 points to calculate transformation matrix
                tmat = utils.solve_transformation(p1,p2,p3,p1T,p2T,p3T)
                self.imgaxes.transformation = Transformation(matrix=tmat)

            self.textbox.setVisible(False)
            self.label_tb.setVisible(False)
            self.textbox.setText("")
            self.plotwdg.setFocus()


    def motion_cb(self,event):
        fig = self.plotwdg.fig
        canvas = fig.canvas

        xmax = fig.get_figwidth()*fig.get_dpi()
        ymax = fig.get_figheight()*fig.get_dpi()

        x_pct = event.x/xmax
        y_pct = event.y/ymax

        inset_size = self.plotwdg.inset_size

        self.plotwdg.ax_inset.set_position([x_pct-inset_size/2,y_pct-inset_size/2,inset_size,inset_size])
        self.update_inset(event.x,event.y)
        canvas.draw()

    def export_data_cb(self):
        outfile = QtWidgets.QFileDialog.getSaveFileName(self.win,'Open file', \
                self.state.workingdir+"/"+'digitized_data.csv','Comma separated variable file (*.csv)')

        if outfile[0] != '':
            self.state.workingdir = os.path.dirname(outfile[0])
            print("Saving file to: ")
            print(outfile[0])
            print("Exporting dataset: ")
            active_dataset = self.get_active_dataset()
            dsT = self.imgaxes.transform_dataset_to_img(active_dataset)
            dsT.sort_dataset()
            print(dsT)
            with open(outfile[0],'w') as f:
                wtr = csv.writer(f,delimiter=',')
                for p in range(len(dsT.points)):
                    wtr.writerow([dsT.points[p].x,dsT.points[p].y])
            print("File saved. ("+str(len(dsT))+" datapoints) ")

    def change_ax_cb(self):
        self.state.mode = "select_axes"
    def edit_dataset_cb(self):
        dlg = EditDatasetDialog()
        active_dataset = self.get_active_dataset()
        old_params = active_dataset.get_params()
        new_params = dlg.open_dlg(old_params)
        active_dataset.load_params(self.plotwdg.ax_main,new_params)
        active_dataset.update_axes_label(self.plotwdg.ax_main)
        active_dataset.update_axes_label(self.plotwdg.ax_inset)
        self.state.active_dataset = new_params.label

        self.win.dataset_combo.setItemText(self.win.dataset_combo.findText(old_params.label),new_params.label)
        
        active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
        active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                self.plotwdg.fig.canvas,markersize=16)


    def new_dataset_cb(self):
        new_dataset = Dataset(DatasetParams("tmp"),points=[])
        self.datasets = np.append(self.datasets,new_dataset)
        self.state.active_dataset = "tmp"
        dlg = EditDatasetDialog()
        active_dataset = self.get_active_dataset()
        old_params = active_dataset.get_params()
        new_params = dlg.open_dlg(old_params)
        active_dataset.load_params(self.plotwdg.ax_main,new_params)
        active_dataset.update_axes_label(self.plotwdg.ax_main)
        active_dataset.update_axes_label(self.plotwdg.ax_inset)
        self.state.active_dataset = new_params.label

        self.win.dataset_combo.insertItem(self.win.dataset_combo.count(),active_dataset.label)
        self.win.dataset_combo.setCurrentText(self.state.active_dataset)
        
        active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
        active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                self.plotwdg.fig.canvas,markersize=16)


    def select_dataset_cb(self,s):
        # s is the string of the newly selected dataset
        self.state.active_dataset = s
        active_dataset = self.get_active_dataset()
        if active_dataset != None:
            active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
            active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                    self.plotwdg.fig.canvas,markersize=16)

    def logx_change_cb(self,i):
        # i is 0 or 2 for unchecked and checked, resp.
        if i == 0:
            self.imgaxes.xaxis.logscale = False
        elif i == 2:
            self.imgaxes.xaxis.logscale = True
        
        # If selecting datapoints, recalculate the output matrix
        if self.state.mode == "select_datapoint":
            p1 = self.imgaxes.xaxis.p1_mpl.get_point_homog_vector()
            p2 = self.imgaxes.xaxis.p2_mpl.get_point_homog_vector()
            p3 = self.imgaxes.yaxis.p1_mpl.get_point_homog_vector()

            # These are transformed coordinates. If logscale, take log of entered value. 
            logx = self.imgaxes.xaxis.logscale
            logy = self.imgaxes.yaxis.logscale

            p1T = self.imgaxes.xaxis.p1_img.get_point_homog_vector()
            p2T = self.imgaxes.xaxis.p2_img.get_point_homog_vector()
            p3T = self.imgaxes.yaxis.p1_img.get_point_homog_vector()
            
            if logx:
                p1T[0][0] = np.log10(p1T[0][0])
                p2T[0][0] = np.log10(p2T[0][0])
                p3T[0][0] = np.log10(p3T[0][0])

            if logy:
                p1T[1][0] = np.log10(p1T[1][0])
                p2T[1][0] = np.log10(p2T[1][0])
                p3T[1][0] = np.log10(p3T[1][0])

            # Now use first 3 points to calculate transformation matrix
            tmat = utils.solve_transformation(p1,p2,p3,p1T,p2T,p3T)
            self.imgaxes.transformation = Transformation(matrix=tmat)
    def logy_change_cb(self,i):
        if i == 0:
            self.imgaxes.yaxis.logscale = False
        elif i == 2:
            self.imgaxes.yaxis.logscale = True

        # If selecting datapoints, recalculate the output matrix
        if self.state.mode == "select_datapoint":
            p1 = self.imgaxes.xaxis.p1_mpl.get_point_homog_vector()
            p2 = self.imgaxes.xaxis.p2_mpl.get_point_homog_vector()
            p3 = self.imgaxes.yaxis.p1_mpl.get_point_homog_vector()

            # These are transformed coordinates. If logscale, take log of entered value. 
            logx = self.imgaxes.xaxis.logscale
            logy = self.imgaxes.yaxis.logscale

            p1T = self.imgaxes.xaxis.p1_img.get_point_homog_vector()
            p2T = self.imgaxes.xaxis.p2_img.get_point_homog_vector()
            p3T = self.imgaxes.yaxis.p1_img.get_point_homog_vector()
            
            if logx:
                p1T[0][0] = np.log10(p1T[0][0])
                p2T[0][0] = np.log10(p2T[0][0])
                p3T[0][0] = np.log10(p3T[0][0])

            if logy:
                p1T[1][0] = np.log10(p1T[1][0])
                p2T[1][0] = np.log10(p2T[1][0])
                p3T[1][0] = np.log10(p3T[1][0])

            # Now use first 3 points to calculate transformation matrix
            tmat = utils.solve_transformation(p1,p2,p3,p1T,p2T,p3T)
            self.imgaxes.transformation = Transformation(matrix=tmat)

    def browse_image_cb(self):
        # Issue data loss warning
        if self.state.hide_browse_warning == False:
            dlg = QtWidgets.QDialog(self.win)
            dlg.btnbox = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel )
            dlg.btnbox.accepted.connect(dlg.accept)
            dlg.btnbox.rejected.connect(dlg.reject)
            dlg.layout = QtWidgets.QVBoxLayout()
            dlg.layout.addWidget(QtWidgets.QLabel("Warning: Selecting a new image will erase all datasets. \n\nThis cannot be undone. \n\nContinue?"))
            dlg.layout.addWidget(dlg.btnbox)
            dlg.setLayout(dlg.layout)

            choose = dlg.exec_()
            if not choose:
                return # If cancelled, stop now

        # Call file browser
        fname_list = QtWidgets.QFileDialog.getOpenFileName(self.win, \
            'Open Image',self.state.workingdir+"/",'Image files (*.png *.jpg *.bmp)')
        fname = fname_list[0]
        if fname == '': # If cancelled, stop now
            return
        
        # Set the new image data, clear the axes
        self.image_name = fname
        self.image_data = ImageFile(fname)
        self.plotwdg.ax_main.clear()
        self.plotwdg.ax_inset.clear()
        self.image_data.plot_image(self.plotwdg.ax_main)
        self.image_data.plot_image(self.plotwdg.ax_inset)
        self.plotwdg.ax_main.set_xticks([])
        self.plotwdg.ax_main.set_yticks([])
        self.plotwdg.ax_inset.axis('off')

        # Reset program state
        self.state.mode = "select_axes"
        self.datasets = np.array([Dataset(DatasetParams("default",markercolor=[0.0,1.0,0.0]),points=[])])
        self.state.active_dataset = self.datasets[0].label
        self.win.dataset_combo.clear()
        self.win.dataset_combo.insertItem(0,self.datasets[0].label)
        self.win.dataset_combo.setCurrentText(self.datasets[0].label)
        self.imgaxes = ImageAxes("axes1")

    def delete_dataset_cb(self):
        # Issue data loss warning
        dlg = QtWidgets.QDialog(self.win)
        dlg.btnbox = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel )
        dlg.btnbox.accepted.connect(dlg.accept)
        dlg.btnbox.rejected.connect(dlg.reject)
        dlg.layout = QtWidgets.QVBoxLayout()
        dlg.layout.addWidget(QtWidgets.QLabel("Warning: Deleting a dataset will erase all points. \n\nThis cannot be undone. \n\nContinue?"))
        dlg.layout.addWidget(dlg.btnbox)
        dlg.setLayout(dlg.layout)

        choose = dlg.exec_()
        if not choose:
            return # If cancelled, stop now

        if len(self.datasets) == 1:
            # Just clean the plot and reset the dataset vector
            self.datasets[0].points = []
            self.datasets[0].plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
            self.datasets[0].plot_dataset(self.plotwdg.ax_inset, \
                    self.plotwdg.fig.canvas,markersize=16)
            self.datasets = np.array([Dataset(DatasetParams("default",markercolor=[0.0,1.0,0.0]),points=[])])
            self.state.active_dataset = self.datasets[0].label
            self.win.dataset_combo.clear()
            self.win.dataset_combo.insertItem(0,self.datasets[0].label)
            self.win.dataset_combo.setCurrentText(self.datasets[0].label)

        else:
            ds_combo_idx = self.win.dataset_combo.findText(self.state.active_dataset)
            ds_idx = -1 # Dataset index, for dataset to be deleted
            for i in range(len(self.datasets)):
                if self.datasets[i].label == self.state.active_dataset:
                    ds_idx = i
            if ds_idx != -1:
                self.datasets[ds_idx].points = []
                self.datasets[ds_idx].plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
                self.datasets[ds_idx].plot_dataset(self.plotwdg.ax_inset, \
                        self.plotwdg.fig.canvas,markersize=16)
                self.datasets[ds_idx] = []
                self.state.active_dataset = self.datasets[0].label
                self.win.dataset_combo.removeItem(ds_combo_idx)
            else:
                print("Error: No active dataset?")


    def browse_csvdata_cb(self):
        pass

    def clear_datapoints_cb(self):
        # Issue data loss warning
        dlg = QtWidgets.QDialog(self.win)
        dlg.btnbox = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel )
        dlg.btnbox.accepted.connect(dlg.accept)
        dlg.btnbox.rejected.connect(dlg.reject)
        dlg.layout = QtWidgets.QVBoxLayout()
        dlg.layout.addWidget(QtWidgets.QLabel("Warning: Clearing a dataset will erase all points. \n\nThis cannot be undone. \n\nContinue?"))
        dlg.layout.addWidget(dlg.btnbox)
        dlg.setLayout(dlg.layout)

        choose = dlg.exec_()
        if not choose:
            return # If cancelled, stop now
        
        # Clear the points and replot
        active_dataset = self.get_active_dataset()
        active_dataset.points = []
        active_dataset.plot_dataset(self.plotwdg.ax_main, self.plotwdg.fig.canvas)
        active_dataset.plot_dataset(self.plotwdg.ax_inset, \
                self.plotwdg.fig.canvas,markersize=16)

    def edit_datapoints_cb(self):
        self.state.mode = "edit_datapoints"


if __name__ == "__main__":
    mainapp = MainApplication()

    mainapp.show()
    sys.exit(mainapp.app.exec_())




