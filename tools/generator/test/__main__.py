import os
import unittest

if __name__ == '__main__':
    suite = unittest.TestLoader().discover(os.path.dirname(__file__))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
