/*
Locally, Typst uses your installed system fonts or embedded fonts in the CLI,
which are:
- Libertinus Serif,
- New Computer Modern,
- New Computer Modern Math, and
- DejaVu Sans Mono.
*/
#set text(
    // font: "New Computer Modern",
    // font: "DejaVu Sans Mono",
    // font: "New Computer Modern Mono",
    font: "Libertinus Serif",
    size: 9pt
)

#set page(margin: (
  top: 1cm,
  bottom: 1cm,
  x: 1cm,
))

#set table.hline(stroke: .6pt)

#table(
    columns: 2,
    inset: 0pt,
    stroke: 1pt,
    align: (center + horizon,center+horizon),
    table.cell(colspan:1,inset:10pt,[
                #table(
            columns: (auto,auto,auto,auto,auto,auto,auto,auto),
            inset: 3pt,
            stroke: 0.2pt,
            align: (x, y) => (
                if x == 0 { center }
                else { right }
            ),
            [*NAME*],[*KM*],[*TIME*],[*HM*],[*DIST*],[*D+*],[*SLOPE*],[*DESCRIPTION*],
             [],[ 0.0],[08:00],[  693 m],[0.0],[    0],[0.0%],[], 
             [],[25.3],[09:41],[ 1025 m],[25.3],[  375],[1.5%],[], 
             [K1
],[29.8],[09:59],[  868 m],[4.5],[    9],[0.2%],[], 
             [],[30.1],[10:00],[  859 m],[0.4],[    0],[0.0%],[], 
             [],[46.0],[11:04],[  988 m],[15.9],[  325],[2.0%],[], 
             [],[57.0],[11:48],[  510 m],[11.0],[   16],[0.1%],[], 
             [],[62.0],[12:08],[  628 m],[5.0],[  155],[3.1%],[], 
             [],[65.4],[12:21],[  354 m],[3.4],[    0],[0.0%],[], 
             [K2
],[72.1],[12:48],[  217 m],[6.6],[   16],[0.2%],[], 
             [],[73.4],[12:53],[  216 m],[1.4],[    2],[0.1%],[], 
             [],[87.9],[13:51],[  803 m],[14.5],[  722],[5.0%],[], 
             [],[95.6],[14:22],[  312 m],[7.6],[    7],[0.1%],[], 
             [],[108.3],[15:13],[  764 m],[12.7],[  468],[3.7%],[], 
             [],[114.3],[15:37],[  393 m],[6.0],[    4],[0.1%],[], 
        )

    ]),
    table.hline()
)

#table(
    columns: 2,
    inset: 0pt,
    stroke: 1pt,
    align: (center + horizon,center+horizon),
    table.cell(colspan:1,inset:10pt,[
                #table(
            columns: (auto,auto,auto,auto,auto,auto,auto,auto),
            inset: 3pt,
            stroke: 0.2pt,
            align: (x, y) => (
                if x == 0 { center }
                else { right }
            ),
            [*NAME*],[*KM*],[*TIME*],[*HM*],[*DIST*],[*D+*],[*SLOPE*],[*DESCRIPTION*],
             [],[87.9],[13:51],[  803 m],[14.5],[  722],[5.0%],[], 
             [],[95.6],[14:22],[  312 m],[7.6],[    7],[0.1%],[], 
             [],[108.3],[15:13],[  764 m],[12.7],[  468],[3.7%],[], 
             [],[114.3],[15:37],[  393 m],[6.0],[    4],[0.1%],[], 
             [],[125.0],[16:20],[  968 m],[10.7],[  581],[5.4%],[], 
             [],[137.0],[17:07],[  597 m],[11.9],[   22],[0.2%],[], 
             [K3],[137.4],[17:09],[  598 m],[0.4],[    0],[0.0%],[], 
             [],[150.1],[18:00],[  961 m],[12.8],[  416],[3.3%],[], 
             [],[156.5],[18:25],[  622 m],[6.3],[   13],[0.2%],[], 
             [],[175.4],[19:41],[  293 m],[18.9],[   60],[0.3%],[], 
             [],[182.9],[20:11],[  698 m],[7.5],[  399],[5.3%],[], 
             [],[187.8],[20:31],[  303 m],[4.9],[    2],[0.0%],[], 
             [],[195.0],[21:00],[  164 m],[7.3],[  112],[1.5%],[], 
             [],[201.5],[21:26],[  426 m],[6.5],[  279],[4.3%],[], 
             [],[209.1],[21:56],[  118 m],[7.6],[    3],[0.0%],[], 
        )

    ]),
    table.hline()
)

#table(
    columns: 2,
    inset: 0pt,
    stroke: 1pt,
    align: (center + horizon,center+horizon),
    table.cell(colspan:1,inset:10pt,[
                #table(
            columns: (auto,auto,auto,auto,auto,auto,auto,auto),
            inset: 3pt,
            stroke: 0.2pt,
            align: (x, y) => (
                if x == 0 { center }
                else { right }
            ),
            [*NAME*],[*KM*],[*TIME*],[*HM*],[*DIST*],[*D+*],[*SLOPE*],[*DESCRIPTION*],
             [],[182.9],[20:11],[  698 m],[7.5],[  399],[5.3%],[], 
             [],[187.8],[20:31],[  303 m],[4.9],[    2],[0.0%],[], 
             [],[195.0],[21:00],[  164 m],[7.3],[  112],[1.5%],[], 
             [],[201.5],[21:26],[  426 m],[6.5],[  279],[4.3%],[], 
             [],[209.1],[21:56],[  118 m],[7.6],[    3],[0.0%],[], 
             [K4],[252.4],[00:49],[  103 m],[43.3],[  123],[0.3%],[], 
             [],[252.4],[00:49],[  103 m],[0.0],[    0],[0.0%],[], 
        )

    ]),
    table.hline()
)

