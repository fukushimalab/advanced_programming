# プログラミング応用
名古屋工業大学情報工学科3年 プログラミング応用講義資料 テンプレートマッチング課題用プログラム

[説明資料（途中で読み込みが止まるので，右クリックしてダウンロードを推奨）](./docs/advanced_programming_signal_processing.pdf)

## memo for 来年


## 達成目標
ある目的を達成するには，すべての機能を一つの言語だけでプログラムする必要はない．

本課題では，画像処理において，特定の画像が入っているかどうか検査するテンプレートマッチングを，以下3つの機能を組み合わせることを実現する．

* c言語プログラム
* ImageMagick
* シェルスクリプト


C言語プログラムでは，テンプレートマッチング自体が実装してある．
ImageMagickは簡単な画像処理を行う，コマンドラインベースのプログラムである．
シェルスクリプトは，上記二つをうまく呼び出すように結合されコマンドを連続で打つためのスクリプトである．

それぞれを工夫することで，最短時間で高い検出率を達成することを目指す．

## マニュアル
1. makeでC言語プログラムのmatchingを生成

2. テンプレートマッチングを実行
```shell
time sh run.sh level1
```

3. 結果をチェック

```shell
sh answer.sh result level1
```

## ダウンロード方法

![ダウンロード方法](./docs/image/fig1.png "ダウンロード方法 ")

## ディレクトリ構成
* advanced_programming_signal_processing
	* 演習の本体
	* advanced_programming_signal_processing.slnでmatching関数をVisual Studioでも編集できる．
		* ソリューションのデバックには，この引数を指定してある．`imgproc/level1_000.ppm  level1/ocean_beach_kinzokutanchi.ppm 0 1.0 cp`
	* ただし，make でコンパイルが基本
* docs
	* 説明用のパワーポイントやpdfファイルが入ってる
* dataset
	* 演習のためのデータセットを作るプログラム（演習するときは必要ない）