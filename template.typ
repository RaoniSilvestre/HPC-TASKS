#let projeto(titulo: "Título Padrão", corpo) = {
  set page(
    paper: "a4",
    margin: (x: 2cm, y: 2.5cm),
    header: align(right, text(8pt, gray)[#titulo]),
  )

  show raw.where(block: true): it => {
    block(
      width: 100%,
      fill: luma(248),
      inset: 10pt,
      radius: 4pt,
      stroke: 0.5pt + luma(200),
      breakable: true, 
      it
    )
  }

  set raw(lang: "c")
  [
    #v(1em) 
    #align(center)[
      #text(20pt, weight: "bold")[#titulo]
    ]
    #v(2em)

    #corpo
  ]
}

