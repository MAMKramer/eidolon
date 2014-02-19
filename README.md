Eidolon
=======
Eidolon (Greek "image, idol, double, apparition, phantom, ghost") enables the creation of interactive applications that are steered by the user's mirrored image on a semi-transparent surface. Utilizing depth-imaging hardware, Eidolon computes the perception of the mirrored image with respect to the user's eye position to perfectly map it with any displayed content.

Setup
-----
The intended setup consists of a semi-transparent reflective surface the user stands in front of and content being displayed from the back by using projection or simply screens. The surface enables the user to see both his reflection and the content. A NiTE-enabled (www.primesense.org) depth-imaging device, such as the Asus xTion is needed to detect the user.

What it does
------------
Using the skeleton data obtained through NiTE, the user's reflected perception of herself on the reflective surface is computed and made available to subsequent visualization. By configuring the screen dimensions and position, Eidolon computes the joint positions for a visualization already in screen coordinates (pixels), no need to fiddle with projection matrices here.

Utility
-------
Eidolon comes with a simple transmission library that requires the visualization programmer to only implement some callbacks to receive the skeleton data - no network programming necessary.

Tutorial / Documentation
--------
The Doxyfile can be found in /doc, currently only those parts exposed in the transmission lib are documented. To be continued.
Tutorial coming soon.

Questions?
----------
Drop a mail at ottona (a) aec.at or use Jabber/XMPP ottona (a) jabber.at

License
-------

CADET - Center for Advances in Digital Entertainment Technologies Copyright 2011 University of Applied Science Salzburg / MultiMediaTechnology & Ars Electronica Futurelab

http://www.cadet.at http://multimediatechnology.at http://research.aec.at

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

CADET - Center for Advances in Digital Entertainment Technologies funded by the Austrian Research Promotion Agency (http://www.ffg.at)
