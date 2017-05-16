# brc_bumper_sticker
GPS-driven LCD 'bumper sticker' that shows road miles to Burning Man

Like most of my one-off projects, this was scraped together from pieces of other projects.  The end result can be seen at http://imgur.com/a/xNw4f and I'll post hardware details if I can find the information.

The goal was to display miles to go along my intended route, and NOT simply the great circle distance to the destination point.  To this end, I used a Garmin nuvi - maybe a 1300, I've got a drawer full of them so I'm not sure, and it'll work with any model that supports the Fleet Management Interface (FMI) protocol.  The nuvi does all of the real work and computes the distance to go.  This project just translates that into miles and drives some big LCDs I got from Electronic Goldmine or some such.

The LCD circuit board was mechanically etched and the housing is a bit of wall-mounted TV cable raceway.