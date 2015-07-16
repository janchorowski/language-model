from fuel.downloaders.base import default_downloader


def fill_subparser(subparser):
    """Set up a subparser to download the Iris dataset file.

    The Iris dataset file `iris.data` is downloaded from the UCI
    Machine Learning Repository [UCI].

    .. [UCI] https://archive.ics.uci.edu/ml/datasets/Iris

    Parameters
    ----------
    subparser : :class:`argparse.ArgumentParser`
        Subparser handling the iris command.

    """
    subparser.set_defaults(
        func=default_downloader,
        urls=['https://archive.ics.uci.edu/ml/machine-learning-databases/'
              'iris/iris.data'],
        filenames=['iris.data'])
