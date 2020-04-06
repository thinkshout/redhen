<?php

namespace Drupal\redhen_connection\Plugin\Derivative;

use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\StringTranslation\TranslationInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides local task definitions for all entity bundles.
 */
class RedhenConnectionLocalTask extends DeriverBase implements ContainerDeriverInterface {

  use StringTranslationTrait;

  /**
   * The entity manager.
   *
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $entityManager;

  /**
   * Creates an SalesforceMappingLocalTask object.
   *
   * @param \Drupal\Core\Entity\EntityManagerInterface $entity_manager
   *   The entity manager.
   * @param \Drupal\Core\StringTranslation\TranslationInterface $string_translation
   *   The translation manager.
   */
  public function __construct(EntityManagerInterface $entity_manager, TranslationInterface $string_translation) {
    $this->entityManager = $entity_manager;
    $this->stringTranslation = $string_translation;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static(
      $container->get('entity.manager'),
      $container->get('string_translation')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    $this->derivatives = [];

    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      if (!($has_canonical_path = $entity_type->hasLinkTemplate('redhen_connection'))) {
        continue;
      }
      $this->derivatives["$entity_type_id.redhen_connection_tab"] = [
        'route_name' => "entity.$entity_type_id.redhen_connection",
        'title' => $this->t('Connections'),
        'base_route' => "entity.$entity_type_id.canonical",
        'weight' => 200,
      ] + $base_plugin_definition;
      $this->derivatives["$entity_type_id.redhen_connection"] = [
        'route_name' => "entity.$entity_type_id.redhen_connection",
        'weight' => 200,
        'title' => $this->t('View'),
        'parent_id' => "redhen_connection.entities:$entity_type_id.redhen_connection_tab",
      ] + $base_plugin_definition;
    }

    return $this->derivatives;
  }

}
