# division_baumkuchen

## 使い方
```
 ./run dirname rdivision rtheta zcut
```

* `dirname`: owner, neighbour, points, faces があるディレクトリの指定 **最後にスラッシュが必須**
* `rdivision`: 半径方向の分割数
* `rtheta`: 角度方向の分割数(ただし, 端と端はmergeされる)
* `zcut`: 一度切る高さの指定

### 出力ファイル
`cellDecomposition` がカレントディレクトリに出る
これをこのまま`constant/`にペーストすれば使える

### 例
```
 ./run ../ 5 5 0.04
```
