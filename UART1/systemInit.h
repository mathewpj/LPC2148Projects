/***************************************************************************************************
                                   ExploreEmbedded    
****************************************************************************************************
 * File:   systemInit.h
 * Version: 15.1
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains standard util macros, typedefs and constants

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 


GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Errors and omissions should be reported to codelibraries@exploreembedded.com
 **************************************************************************************************/



#ifndef  _SYSTEMINIT_H_
#define  _SYSTEMINIT_H_




/***************************************************************************************************
                           SystemInit and Clock configuration.
***************************************************************************************************/                           
#define pclk 60000000UL  // PCLK set to 60Mhz using PLL configuration(refer systemInit.c file)
void SystemInit(void);
/**************************************************************************************************/





#endif    

