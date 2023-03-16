<?php

declare(strict_types=1);

namespace Drupal\google_tag;

use Drupal\Component\Plugin\Exception\PluginException;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\google_tag\Annotation\GoogleTagEvent;
use Drupal\google_tag\Plugin\GoogleTag\Event\GoogleTagEventInterface;

/**
 * Plugin manager for Google tag event plugins.
 */
final class GoogleTagEventManager extends DefaultPluginManager {

  /**
   * {@inheritDoc}
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct(
      'Plugin/GoogleTag/Event',
      $namespaces,
      $module_handler,
      GoogleTagEventInterface::class,
      GoogleTagEvent::class,
    );
    $this->alterInfo('google_tag_event_info');
    $this->setCacheBackend($cache_backend, 'google_tag_event_plugins');
  }

  /**
   * {@inheritDoc}
   */
  public function processDefinition(&$definition, $plugin_id) {
    parent::processDefinition($definition, $plugin_id);
    foreach (['id', 'label'] as $required_property) {
      if (empty($definition[$required_property])) {
        throw new PluginException(sprintf('The tag event %s must define the %s property.', $plugin_id, $required_property));
      }
    }
    if (empty($definition['event_name'])) {
      $definition['event_name'] = $definition['id'];
    }
  }

  /**
   * {@inheritDoc}
   */
  protected function findDefinitions(): array {
    $definitions = parent::findDefinitions();
    foreach ($definitions as $plugin_id => $plugin_definition) {
      $dependency = $plugin_definition['dependency'] ?? '';
      if ($dependency === '') {
        continue;
      }
      if (!$this->moduleHandler->moduleExists($dependency)) {
        unset($definitions[$plugin_id]);
      }
    }
    return $definitions;
  }

}
