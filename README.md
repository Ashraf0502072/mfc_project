# mfc_project
Delivering an accurate representation of the lane ahead of an autonomously driving vehicle is 
one of the key functionalities of a good ADAS perception system. This statement holds 
especially for driving on a highway. Functions such as lane keeping assistance rely on a proper 
representation of the lanes from the perception subsystem. To achieve such a proper 
representation, information from Vector map data are taken into consideration for this Master
thesis.

For graphical representation of lane level road along with different types of crossing and traffic 
sign in this thesis Visual studio and c++ programming is used. For implementation Microsoft 
Foundation class and a Windows data structure containing information about the drawing 
attributes of Visual studio is used which is called Device Context.

In the final GUI the road view has four features. Firstly, the road view with multilane is 
displayed . The centerline of multi lane road is displayed with solid white line, on the other 
hand lane is separated with dotted line. If the number of lane is one, the centerline is divided by 
dotted line. Second feature is Vehicle position is changed with the increase or decrease on the 
number of lane. The vehicle is a bitmap file which will be shown from resource file. In the third 
feature on the right side of the road different types of crossing Signal and on the left side of the 
road others attribute of traffic signal is displayed. Finally on the left of the window a scale is 
drawn from the road displayed ahead of the road, which can vary from 100m to 1000m.
