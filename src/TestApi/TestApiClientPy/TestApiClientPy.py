
import TestApi

itemCollection = TestApi.ItemCollection()

itemCollection.addItem(1, 'string')
itemCollection.addItem(98, "bob")
itemCollection.addItem(76, "joe")
itemCollection.addItem(54, "rick")
itemCollection.addItem(32, "jim")
itemCollection.addItem(10, "jack")
itemCollection.addItem(-123, "john")

for item in itemCollection.items():
    print('Item: i={}, s={}'.format(item[0], item[1]))

