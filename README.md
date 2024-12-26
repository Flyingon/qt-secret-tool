
### 调试
```
PRAGMA key='your_secure_password';

INSERT INTO t_secret (key, secret, created_at, updated_at) VALUES ('DDDD', 'HHASHADJ我去取钱打卡\n 阿卡打卡', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);

INSERT INTO t_secret (key, secret, created_at, updated_at) VALUES ('AAAA', 'BBBB', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);
```


### 打包
```
macdeployqt qt-secret-tool.app -verbose=3 
```


### 重新签名
```
security find-identity -v -p codesigning

codesign --remove-signature qt-secret-tool.app

codesign -s "FBA5E6E046C1EE499BFBD647816A80A4FB6BF454" qt-secret-tool.app
codesign --deep --force --sign "FBA5E6E046C1EE499BFBD647816A80A4FB6BF454" qt-secret-tool.app

```