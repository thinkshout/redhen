<?php

namespace Drupal\redhen_connection\Plugin\Derivative;

use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Entity\EntityLastInstalledSchemaRepositoryInterface;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\redhen_connection\ConnectionService;
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
  protected $service;

  /**
   * Creates an SalesforceMappingLocalTask object.
   *
   * @param ConnectionService $service
   *   The service manager.
   * @param \Drupal\Core\StringTranslation\TranslationInterface $string_translation
   *   The translation manager.
   */
  public function __construct(ConnectionService $service, TranslationInterface $string_translation) {
    $this->service = $service;
    $this->stringTranslation = $string_translation;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static(
      $container->get('redhen_connection.connections'),
      $container->get('string_translation')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    $this->derivatives = [];

    foreach ($this->service->getAllConnectionEntityTypes() as $entity_type_id => $entity_type) {
      if (!($entity_type->hasLinkTemplate('redhen_connection'))) {
        continue;
      }
      $this->derivatives["$entity_type_id.redhen_connection_tab"] = [
        'route_name' => "entity.$entity_type_id.redhen_connection",
        'title' => $this->t('Connections'),
        'base_route' => "entity.$entity_type_id.canonical",
        'weight' => 20,
      ] + $base_plugin_definition;
    }

    return $this->derivatives;
  }

}
