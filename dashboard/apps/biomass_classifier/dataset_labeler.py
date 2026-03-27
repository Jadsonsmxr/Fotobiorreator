from __future__ import annotations

import argparse
import glob
from pathlib import Path

import pandas as pd

from apps.biomass_classifier.service import DEFAULT_TRAINING_DATASET_PATH, VALID_CLASSES, extract_features


def label_images(image_paths: list[str]) -> list[dict[str, object]]:
    labeled_records = []

    for image_path in image_paths:
        path = Path(image_path)
        if not path.is_file():
            print(f"[aviso] Arquivo não encontrado: {path}")
            continue

        features = extract_features(path)

        print(f"\nArquivo: {path.name}")
        print(f"  mean_green_index: {features['mean_green_index']:.4f}")
        print(f"  mean_gray      : {features['mean_gray']:.1f}")
        print(f"  std_color      : {features['std_color']:.2f}")
        print(f"  mean_hue       : {features['mean_hue']:.1f}°")

        while True:
            options = "  ".join(f"[{index + 1}] {value}" for index, value in enumerate(VALID_CLASSES))
            user_input = input(f"  Classe? {options} -> ").strip()

            if user_input.isdigit() and 1 <= int(user_input) <= len(VALID_CLASSES):
                status = VALID_CLASSES[int(user_input) - 1]
                break

            if user_input in VALID_CLASSES:
                status = user_input
                break

            print("  Opção inválida. Digite o número ou o nome da classe.")

        labeled_records.append({**features, "status": status, "arquivo": path.name})
        print(f"  Rotulado como: {status}")

    return labeled_records


def save_training_dataset(records: list[dict[str, object]]) -> None:
    new_frame = pd.DataFrame(records)
    required_columns = ["mean_green_index", "mean_gray", "std_color", "mean_hue", "status", "arquivo"]
    new_frame = new_frame[required_columns]

    if DEFAULT_TRAINING_DATASET_PATH.exists():
        existing_frame = pd.read_csv(DEFAULT_TRAINING_DATASET_PATH)
        combined_frame = pd.concat([existing_frame, new_frame], ignore_index=True)
        mode_label = "atualizado"
    else:
        combined_frame = new_frame
        mode_label = "criado"

    combined_frame.to_csv(DEFAULT_TRAINING_DATASET_PATH, index=False)
    print(
        f"\nDataset {mode_label}: {DEFAULT_TRAINING_DATASET_PATH} "
        f"({len(combined_frame)} amostras no total)"
    )
    print(combined_frame["status"].value_counts().to_string())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Rotulador de imagens para o classificador de biomassa")
    parser.add_argument("imagens", nargs="*", help="Caminhos das imagens")
    parser.add_argument("--pasta", type=str, help="Pasta com imagens JPEG/PNG")
    args = parser.parse_args()

    image_paths = list(args.imagens)
    if args.pasta:
        for pattern in ("*.jpg", "*.jpeg", "*.png", "*.JPG", "*.JPEG", "*.PNG"):
            image_paths.extend(glob.glob(str(Path(args.pasta) / pattern)))

    if not image_paths:
        print("Informe imagens ou use --pasta <diretório>")
        raise SystemExit(1)

    print(f"=== Rotulador de biomassa — {len(image_paths)} imagem(ns) ===")
    labeled_records = label_images(image_paths)

    if labeled_records:
        save_training_dataset(labeled_records)
        print("\nPróximo passo:")
        print("  python -m apps.biomass_classifier.dataset_labeler --help")
