# templateMatching.sln
データセットのジェネレータ

## 使い方
main関数にて，生成したいレベルのifブロックをtrueにする．
詳しいことは下記参照

## main.cpp メモ
### 構造体
- option
  - 埋め込みで使用するパラメータ
  - angle 回転角度
  - tx x移動量
  - ty y移動量

### 変数
- int genenum
  - 生成画像枚数
- vector\<string> src_names
  - 元画像のファイル名のvector
  - 元画像を追加する場合はこれにファイル名をpush
- vector\<string> temp_names
  - テンプレート画像のファイル名のvector
  - テンプレート画像を追加する場合はこれにファイル名をpush

### 関数
- int load_image(Mat& src, const vector<string> src_names, Mat& temp, const vector<string> temp_names, bool isLevel4 = false)
    - src_namesとtemp_namesからランダムにファイルを選んでsrcとtempに読み込み
    - 戻り値は選択したテンプレート画像の番号
    - 引数isLevel4をtrueにするとテンプレート画像のアルファチャンネル読み込み(level4で使用)
- void addImpulseNoise(const Mat src, Mat& dest, double x)
  - 画像にインパルスノイズを付与
  - 引数xはノイズの発生確率(0<=x<=1)
- void embedImage(const cv::Mat src, cv::Mat& dest, cv::Mat& embeddedImage, const option op)
  - 画像埋め込み用関数
  - srcにembeddedImageをopの数値で埋め込んでdestに出力
  - テンプレート画像にアルファチャネルがある場合は背景透過
- void outputText(string outputFilename, string embeddedImageName, int tx, int ty, Mat embedImage, int angle)
  - 正解をテキスト出力
- void generateImage(const Mat src, Mat& dest, const Mat embeddedImage, option op, bool isSeamless = false)
  - オプションでシームレスクローニングを選択可の画像埋め込み関数(おそらく)
  - 使っていない
- void generateImageLvInf(const Mat src, Mat& dest, Mat embeddedImage,Mat& temp, int& angle,int& tx,int& ty)
  - レベルinf用の画像生成(おそらく)
  - エラーの修正をしていないため使用不可
  - 使っていない
