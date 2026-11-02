// VR1: 6=3V, 5=P0, 4=GND
//
// P0ゾーン:
// 0～100    : 4側の新ストローク
// 101～469  : 4側の停止帯
// 470～553  : 中央
// 554～922  : 6側の停止帯
// 923～1023 : 6側の新ストローク
//
// 6側ストローク -> ド→ミ→ソ
// 4側ストローク -> ソ→ミ→ド
// 反対側へ少し倒すだけで広い停止帯に入り、即停止しやすくしています。
// 新しい入力がなければ約3秒で疑似フェードアウト。
guitarchord.setDelay(100)
guitarchord.assignChord(guitarchord.ChordButton.NoButton, guitarchord.Chord.C)
guitarchord.assignChord(guitarchord.ChordButton.Button1, guitarchord.Chord.G)
guitarchord.assignChord(guitarchord.ChordButton.Button2, guitarchord.Chord.Am)
guitarchord.assignChord(guitarchord.ChordButton.Button1Button2, guitarchord.Chord.F)
